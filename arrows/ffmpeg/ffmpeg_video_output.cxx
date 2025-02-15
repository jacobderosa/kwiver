// This file is part of KWIVER, and is distributed under the
// OSI-approved BSD 3-Clause License. See top-level LICENSE file or
// https://github.com/Kitware/kwiver/blob/master/LICENSE for details.

/// \file
/// Implementation of FFmpeg video writer.

#include "arrows/ffmpeg/ffmpeg_cuda.h"
#include "arrows/ffmpeg/ffmpeg_init.h"
#include "arrows/ffmpeg/ffmpeg_video_output.h"
#include "arrows/ffmpeg/ffmpeg_video_raw_image.h"
#include "arrows/ffmpeg/ffmpeg_video_settings.h"

#include <vital/optional.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace kv = kwiver::vital;

namespace kwiver {

namespace arrows {

namespace ffmpeg {

// ----------------------------------------------------------------------------
class ffmpeg_video_output::impl
{
public:
  struct open_video_state {
    open_video_state(
      impl& parent, std::string const& video_name,
      ffmpeg_video_settings const& settings );
    open_video_state( open_video_state const& ) = delete;
    open_video_state( open_video_state&& ) = default;
    ~open_video_state();

    open_video_state&
    operator=( open_video_state const& ) = delete;
    open_video_state&
    operator=( open_video_state&& ) = default;

    bool try_codec( ffmpeg_video_settings const& settings );

    void add_image(
      kv::image_container_sptr const& image, kv::timestamp const& ts );
    void add_image( vital::video_raw_image const& image );

    bool write_next_packet();
    void write_remaining_packets();

    int64_t next_video_pts() const;

    impl* parent;

    size_t frame_count;
    format_context_uptr format_context;
    AVOutputFormat* output_format;
    AVStream* video_stream;
    AVStream* metadata_stream;
    codec_context_uptr codec_context;
    AVCodec const* codec;
    sws_context_uptr image_conversion_context;
  };

  impl();
  ~impl();

  bool is_open() const;
  void assert_open( std::string const& fn_name ) const;

  void hardware_init();
  void cuda_init();

  AVHWDeviceContext* hardware_device() const;
#ifdef KWIVER_ENABLE_FFMPEG_CUDA
  AVCUDADeviceContext* cuda_device() const;
#endif

  kv::logger_handle_t logger;

  hardware_device_context_uptr hardware_device_context;

  size_t width;
  size_t height;
  AVRational frame_rate;
  std::string codec_name;
  size_t bitrate;
  bool cuda_enabled;
  int cuda_device_index;

  kv::optional< open_video_state > video;
};

// ----------------------------------------------------------------------------
ffmpeg_video_output::impl
::impl()
  : logger{},
    width{ 0 },
    height{ 0 },
    frame_rate{ 0, 1 },
    codec_name{},
    bitrate{ 0 },
#ifdef KWIVER_ENABLE_FFMPEG_CUDA
    cuda_enabled{ true },
#else
    cuda_enabled{ false },
#endif
    cuda_device_index{ 0 },
    video{}
{
  ffmpeg_init();
}

// ----------------------------------------------------------------------------
ffmpeg_video_output::impl
::~impl()
{}

// ----------------------------------------------------------------------------
bool
ffmpeg_video_output::impl
::is_open() const
{
  return video.has_value();
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output::impl
::assert_open( std::string const& fn_name ) const
{
  if( !is_open() )
  {
    VITAL_THROW(
      kv::file_write_exception, "<unknown file>",
      "Function " + fn_name + " called before successful open()" );
  }
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output::impl
::hardware_init()
{
  if( !hardware_device_context && cuda_enabled )
  {
    try
    {
      cuda_init();
    }
    catch( std::exception const& e )
    {
      LOG_ERROR( logger, "CUDA initialization failed: " << e.what() );
    }
  }
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output::impl
::cuda_init()
{
#ifdef KWIVER_ENABLE_FFMPEG_CUDA
  hardware_device_context =
    std::move( cuda_create_context( cuda_device_index ) );
#else
  LOG_DEBUG(
    logger,
    "Could not initialize CUDA: Not compiled with KWIVER_ENABLE_CUDA" );
#endif
}

// ----------------------------------------------------------------------------
AVHWDeviceContext*
ffmpeg_video_output::impl
::hardware_device() const
{
  if( !hardware_device_context )
  {
    return nullptr;
  }
  return reinterpret_cast< AVHWDeviceContext* >(
    hardware_device_context->data );
}

// ----------------------------------------------------------------------------
#ifdef KWIVER_ENABLE_FFMPEG_CUDA
AVCUDADeviceContext*
ffmpeg_video_output::impl
::cuda_device() const
{
  if( !hardware_device() ||
      hardware_device()->type != AV_HWDEVICE_TYPE_CUDA )
  {
    return nullptr;
  }
  return static_cast< AVCUDADeviceContext* >( hardware_device()->hwctx );
}
#endif

// ----------------------------------------------------------------------------
ffmpeg_video_output
::ffmpeg_video_output() : d{ new impl{} }
{
  attach_logger( "ffmpeg_video_output" );
  d->logger = logger();

  set_capability( kv::algo::video_output::SUPPORTS_FRAME_RATE, true );
  set_capability( kv::algo::video_output::SUPPORTS_FRAME_TIME, true );
  set_capability( kv::algo::video_output::SUPPORTS_METADATA, true );
}

// ----------------------------------------------------------------------------
ffmpeg_video_output::~ffmpeg_video_output()
{
  close();
}

// ----------------------------------------------------------------------------
vital::config_block_sptr
ffmpeg_video_output
::get_configuration() const
{
  auto config = vital::algorithm::get_configuration();

  config->set_value(
    "width", d->width,
    "Output width in pixels."
  );
  config->set_value(
    "height", d->height,
    "Output height in pixels."
  );
  config->set_value(
    "frame_rate_num", d->frame_rate.num,
    "Integral numerator of the output frame rate."
  );
  config->set_value(
    "frame_rate_den", d->frame_rate.den,
    "Integral denominator of the output frame rate. Defaults to 1."
  );
  config->set_value(
    "codec_name", d->codec_name,
    "String identifying the codec to use."
  );
  config->set_value(
    "bitrate", d->bitrate,
    "Desired bitrate in bits per second."
  );

  config->set_value(
    "cuda_enabled", d->cuda_enabled,
    "When set to true, uses CUDA/NVENC to accelerate video encoding."
  );
  config->set_value(
    "cuda_device_index", d->cuda_device_index,
    "Integer index of the CUDA-enabled device to use for encoding. "
    "Defaults to 0."
  );

  return config;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::set_configuration( kv::config_block_sptr config )
{
  auto existing_config = vital::algorithm::get_configuration();
  existing_config->merge_config( config );

  d->width = config->get_value< size_t >( "width", d->width );
  d->height = config->get_value< size_t >( "height", d->height );
  d->frame_rate.num =
    config->get_value< size_t >( "frame_rate_num", d->frame_rate.num );
  if( config->has_value( "frame_rate_num" ) )
  {
    d->frame_rate.den = 1;
  }
  d->frame_rate.den =
    config->get_value< size_t >( "frame_rate_den", d->frame_rate.den );
  d->codec_name =
    config->get_value< std::string >( "codec_name", d->codec_name );
  d->bitrate = config->get_value< size_t >( "bitrate", d->bitrate );

  d->cuda_enabled =
    config->get_value< bool >(
      "cuda_enabled", d->cuda_enabled );

  if( !d->cuda_enabled && d->hardware_device() &&
      d->hardware_device()->type == AV_HWDEVICE_TYPE_CUDA )
  {
    // Turn off the active CUDA instance
    d->hardware_device_context.reset();
  }

  d->cuda_device_index =
    config->get_value< int >(
      "cuda_device_index", d->cuda_device_index );
}

// ----------------------------------------------------------------------------
bool
ffmpeg_video_output
::check_configuration( kv::config_block_sptr config ) const
{
  return true;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::open(
  std::string video_name, vital::video_settings const* generic_settings )
{
  // Ensure we start from a blank slate
  close();

  ffmpeg_video_settings const default_settings;
  auto settings =
    generic_settings
    ? dynamic_cast< ffmpeg_video_settings const* >( generic_settings )
    : nullptr;
  if( !settings )
  {
    settings = &default_settings;
  }

  d->hardware_init();
  d->video.emplace( *d, video_name, *settings );
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::close()
{
  d->video.reset();
}

// ----------------------------------------------------------------------------
bool
ffmpeg_video_output
::good() const
{
  return d->is_open();
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::add_image( kv::image_container_sptr const& image, kv::timestamp const& ts )
{
  d->assert_open( "add_image()" );
  d->video->add_image( image, ts );
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::add_image( vital::video_raw_image const& image )
{
  d->assert_open( "add_image()" );
  d->video->add_image( image );
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output
::add_metadata( VITAL_UNUSED kwiver::vital::metadata const& md )
{
  // TODO
}

// ----------------------------------------------------------------------------
vital::video_settings_uptr
ffmpeg_video_output
::implementation_settings() const
{
  if( !d->is_open() )
  {
    return nullptr;
  }

  auto const result = new ffmpeg_video_settings{};
  result->frame_rate = d->video->video_stream->avg_frame_rate;
  avcodec_parameters_from_context( result->parameters.get(),
                                   d->video->codec_context.get() );
  result->klv_stream_count = 0; // TODO
  return kwiver::vital::video_settings_uptr{ result };
}

// ----------------------------------------------------------------------------
ffmpeg_video_output::impl::open_video_state
::open_video_state(
  impl& parent, std::string const& video_name,
  ffmpeg_video_settings const& settings )
  : parent{ &parent },
    frame_count{ 0 },
    format_context{ nullptr },
    output_format{ nullptr },
    video_stream{ nullptr },
    metadata_stream{ nullptr },
    codec_context{ nullptr },
    codec{ nullptr },
    image_conversion_context{ nullptr }
{
  // Allocate output format context
  {
    AVFormatContext* tmp = nullptr;
    throw_error_code(
      avformat_alloc_output_context2(
        &tmp, nullptr, nullptr, video_name.c_str() ),
      "Could not allocate format context" );
    format_context.reset( tmp );
  }
  output_format = format_context->oformat;

  // Prioritization scheme for codecs:
  // (1) Match ffmpeg settings passed to constructor if present
  // (2) Match configuration setting if present
  // (3) Choose H.265 and H.264 over other codecs
  // (4) Choose hardware codecs over software codecs
  auto const codec_cmp =
    [&]( AVCodec const* lhs, AVCodec const* rhs ) -> bool {
      return
        std::make_tuple(
          lhs->id == settings.parameters->codec_id,
          lhs->name == parent.codec_name,
          lhs->id == AV_CODEC_ID_H265,
          lhs->id == AV_CODEC_ID_H264,
          is_hardware_codec( lhs ) ) >
        std::make_tuple(
          rhs->id == settings.parameters->codec_id,
          rhs->name == parent.codec_name,
          rhs->id == AV_CODEC_ID_H265,
          rhs->id == AV_CODEC_ID_H264,
          is_hardware_codec( rhs ) );
    };

  std::multiset<
    AVCodec const*, std::function< bool( AVCodec const*, AVCodec const* ) > >
  possible_codecs{ codec_cmp };

  // Find all compatible CUDA codecs
#ifdef KWIVER_ENABLE_FFMPEG_CUDA
  if( parent.cuda_device() )
  {
    auto const cuda_codecs =
      cuda_find_encoders( *output_format, *settings.parameters );
    possible_codecs.insert( cuda_codecs.begin(), cuda_codecs.end() );
  }
#endif

  // Find all compatible software codecs
  AVCodec const* codec_ptr = nullptr;
#if LIBAVCODEC_VERSION_MAJOR > 57
  for( void* it = nullptr; ( codec_ptr = av_codec_iterate( &it ) ); )
#else
  while( ( codec_ptr = av_codec_next( codec_ptr ) ) )
#endif
  {
    if( av_codec_is_encoder( codec_ptr ) &&
        !is_hardware_codec( codec_ptr ) &&
        !( codec_ptr->capabilities & AV_CODEC_CAP_EXPERIMENTAL ) &&
        format_supports_codec( output_format, codec_ptr->id ) )
    {
      possible_codecs.emplace( codec_ptr );
    }
  }

  // Find the first compatible codec that works, in priority order
  for( auto const possible_codec : possible_codecs )
  {
    codec = possible_codec;
    if( try_codec( settings ) )
    {
      break;
    }
    else
    {
      codec = nullptr;
    }
  }

  throw_error_null(
    codec,
    "Could not open video with any known output codec. ",
    possible_codecs.size(), " codecs were tried." );
  LOG_INFO(
    parent.logger, "Using output codec " << pretty_codec_name( codec ) );

  av_dump_format(
    format_context.get(), video_stream->index, video_name.c_str(), 1 );

  // Open streams
  throw_error_code(
    avio_open( &format_context->pb, video_name.c_str(), AVIO_FLAG_WRITE ),
    "Could not open `", video_name, "` for writing" );

  auto const output_status =
    avformat_init_output( format_context.get(), nullptr );
  if( output_status == AVSTREAM_INIT_IN_WRITE_HEADER )
  {
    throw_error_code(
      avformat_write_header( format_context.get(), nullptr ),
      "Could not write video header" );
  }
  throw_error_code( output_status, "Could not initialize output stream" );
}

// ----------------------------------------------------------------------------
ffmpeg_video_output::impl::open_video_state
::~open_video_state()
{
  if( format_context )
  {
    write_remaining_packets();

    // Write closing bytes of video format
    auto err = av_write_trailer( format_context.get() );
    if( err < 0 )
    {
      LOG_ERROR(
        parent->logger,
        "Could not write video trailer: " << error_string( err ) );
    }
  }
}

// ----------------------------------------------------------------------------
bool
ffmpeg_video_output::impl::open_video_state
::try_codec( ffmpeg_video_settings const& settings )
{
  LOG_DEBUG(
    parent->logger, "Trying output codec: " << pretty_codec_name( codec ) );

  // Create and configure codec context
  codec_context.reset(
    throw_error_null(
      avcodec_alloc_context3( codec ), "Could not allocate codec context" ) );

  // Fill in fields from given settings
  if( codec->id == settings.parameters->codec_id )
  {
    throw_error_code(
      avcodec_parameters_to_context(
        codec_context.get(), settings.parameters.get() ) );
  }
  else
  {
    codec_context->width = settings.parameters->width;
    codec_context->height = settings.parameters->height;
  }
  codec_context->time_base = av_inv_q( settings.frame_rate );
  codec_context->framerate = settings.frame_rate;

  // Fill in backup parameters from config
  if( codec_context->pix_fmt < 0 )
  {
    // TODO: Add config options so RGB24 is not hardcoded here
    codec_context->pix_fmt = avcodec_find_best_pix_fmt_of_list(
      codec->pix_fmts, AV_PIX_FMT_RGB24, false, nullptr );
  }
  if( codec_context->framerate.num <= 0 )
  {
    codec_context->framerate = parent->frame_rate;
    codec_context->time_base = av_inv_q( parent->frame_rate );
  }
  if( codec_context->width <= 0 )
  {
    codec_context->width = parent->width;
  }
  if( codec_context->height <= 0 )
  {
    codec_context->height = parent->height;
  }
  if( codec_context->bit_rate <= 0 )
  {
    codec_context->bit_rate = parent->bitrate;
  }

  // Ensure we have all the required information
  if( codec_context->width <= 0 || codec_context->height <= 0 ||
      codec_context->framerate.num <= 0 )
  {
    throw_error(
      "FFmpeg video output requires width, height, and frame rate to be "
      "specified prior to calling open()" );
  }

  // Create video stream
  if( output_format->video_codec == AV_CODEC_ID_NONE )
  {
    throw_error( "Output format does not support video" );
  }

  video_stream =
    throw_error_null(
      avformat_new_stream( format_context.get(), codec ),
      "Could not allocate video stream" );
  video_stream->time_base = codec_context->time_base;
  video_stream->codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
  video_stream->codecpar->codec_id = codec->id;
  video_stream->codecpar->width = codec_context->width;
  video_stream->codecpar->height = codec_context->height;
  video_stream->codecpar->format = codec_context->pix_fmt;

  auto const err = avcodec_open2( codec_context.get(), codec, nullptr );
  if( err < 0 )
  {
    LOG_WARN(
      parent->logger,
      "Could not open output codec: " << pretty_codec_name( codec )
      << ": " << error_string( err ) );
    return false;
  }

  return true;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output::impl::open_video_state
::add_image( kv::image_container_sptr const& image,
             VITAL_UNUSED kv::timestamp const& ts )
{
  // Create frame object to represent incoming image
  frame_uptr frame{
    throw_error_null( av_frame_alloc(), "Could not allocate frame" ) };

  // Fill in a few mandatory fields
  frame->width = image->width();
  frame->height = image->height();
  switch( image->depth() )
  {
    case 1: frame->format = AV_PIX_FMT_GRAY8; break;
    case 3: frame->format = AV_PIX_FMT_RGB24; break;
    default:
      throw_error( "Image has unsupported depth: ", image->depth() );
  }

  if( image->get_image().pixel_traits() !=
      kv::image_pixel_traits_of< uint8_t >() )
  {
    // TODO: Is there an existing conversion function somewhere?
    throw_error( "Image has unsupported pixel traits (non-uint8)" );
  }

  // Allocate storage based on those fields
  throw_error_code(
    av_frame_get_buffer( frame.get(), 32 ), "Could not allocate frame data" );

  // Give the frame the raw pixel data
  {
    size_t index = 0;
    auto ptr = static_cast< uint8_t* >( image->get_image().first_pixel() );
    auto const i_step = image->get_image().h_step();
    auto const j_step = image->get_image().w_step();
    auto const k_step = image->get_image().d_step();
    for( size_t i = 0; i < image->height(); ++i )
    {
      for( size_t j = 0; j < image->width(); ++j )
      {
        for( size_t k = 0; k < image->depth(); ++k )
        {
          frame->data[ 0 ][ index++ ] = *ptr;
          ptr += k_step;
        }
        ptr += j_step - k_step * image->depth();
      }
      ptr += i_step - j_step * image->width();
      index += frame->linesize[ 0 ] - image->width() * image->depth();
    }
  }

  // Create frame object to hold the image after conversion to the required
  // pixel format
  frame_uptr converted_frame{
    throw_error_null( av_frame_alloc(), "Could not allocate frame" ) };

  // Fill in a few mandatory fields
  converted_frame->width = image->width();
  converted_frame->height = image->height();
  converted_frame->format = codec_context->pix_fmt;

  // Allocate storage based on those fields
  throw_error_code(
    av_frame_get_buffer( converted_frame.get(), 32 ),
    "Could not allocate frame data" );

  // Specify which conversion to perform
  image_conversion_context.reset(
    throw_error_null(
      sws_getCachedContext(
        image_conversion_context.release(),
        image->width(), image->height(),
        static_cast< AVPixelFormat >( frame->format ),
        image->width(), image->height(),
        static_cast< AVPixelFormat >( converted_frame->format ),
        SWS_BICUBIC, nullptr, nullptr, nullptr ),
      "Could not create image conversion context" ) );

  // Convert the pixel format
  throw_error_code(
    sws_scale(
      image_conversion_context.get(), frame->data, frame->linesize,
      0, image->height(), converted_frame->data, converted_frame->linesize ),
    "Could not convert frame image to target pixel format" );

  // Try to send image to video encoder
  converted_frame->pts = next_video_pts();
  throw_error_code(
    avcodec_send_frame( codec_context.get(), converted_frame.get() ),
    "Could not send frame to encoder" );

  // Write encoded packets out
  while( write_next_packet() );

  ++frame_count;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output::impl::open_video_state
::add_image( kv::video_raw_image const& image )
{
  auto const& ffmpeg_image =
    dynamic_cast< ffmpeg_video_raw_image const& >( image );
  for( auto const& packet : ffmpeg_image.packets )
  {
    throw_error_code(
      av_interleaved_write_frame( format_context.get(), packet.get() ),
      "Could not write frame to file" );
  }
  ++frame_count;
}

// ----------------------------------------------------------------------------
bool
ffmpeg_video_output::impl::open_video_state
::write_next_packet()
{
  packet_uptr packet{
    throw_error_null( av_packet_alloc(), "Could not allocate packet" ) };

  // Attempt to read next encoded packet
  auto const err = avcodec_receive_packet( codec_context.get(), packet.get() );

  if( err == AVERROR( EAGAIN ) || err == AVERROR_EOF )
  {
    // Failed expectedly: no packet to read
    return false;
  }
  throw_error_code( err, "Could not get next packet from encoder" );

  // Succeeded; write to file
  throw_error_code(
    av_interleaved_write_frame( format_context.get(), packet.get() ),
    "Could not write frame to file" );

  return true;
}

// ----------------------------------------------------------------------------
void
ffmpeg_video_output::impl::open_video_state
::write_remaining_packets()
{
  // Enter "draining mode" - i.e. signal end of file
  avcodec_send_frame( codec_context.get(), nullptr );

  while( write_next_packet() ) {}
}

// ----------------------------------------------------------------------------
int64_t
ffmpeg_video_output::impl::open_video_state
::next_video_pts() const
{
  return static_cast< int64_t >(
    frame_count / av_q2d( video_stream->time_base ) /
    av_q2d( codec_context->framerate ) + 0.5 );
}

} // namespace ffmpeg

} // namespace arrows

} // namespace kwiver
