/*ckwg +29
 * Copyright 2018 by Kitware, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 *  * Neither name of Kitware, Inc. nor the names of any contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "detected_object_type.h"

namespace kwiver {
namespace arrows {
namespace serialize {
namespace protobuf {

// ----------------------------------------------------------------------------
detected_object_type::
detected_object_type()
{
  m_element_names.insert( DEFAULT_ELEMENT_NAME );

  // Verify that the version of the library that we linked against is
  // compatible with the version of the headers we compiled against.
  GOOGLE_PROTOBUF_VERIFY_VERSION;
}


detected_object_type::
~detected_object_type()
{ }

// ----------------------------------------------------------------------------
std::shared_ptr< std::string >
detected_object_type::
serialize( const data_serializer::serialize_param_t elements )
{
  kwiver::vital::detected_object_type dot =
    kwiver::vital::any_cast< kwiver::vital::detected_object_type > ( elements.at( DEFAULT_ELEMENT_NAME ) );

  std::ostringstream msg;
  msg << "detected_object_type "; // add type tag

  kwiver::protobuf::detected_object_type proto_dot;
  convert_protobuf( dot, proto_dot );

  if ( ! proto_dot.SerializeToOstream( &msg ) )
  {
    // throw something
  }

  return std::make_shared< std::string > ( msg.str() );
}

// ----------------------------------------------------------------------------
vital::algo::data_serializer::deserialize_result_t
detected_object_type::
deserialize( std::shared_ptr< std::string > message )
{
  kwiver::vital::detected_object_type dot;
  std::istringstream msg( *message );

  std::string tag;
  msg >> tag;
  msg.get();  // Eat delimiter

  if (tag != "detected_object_type" )
  {
    LOG_ERROR( logger(), "Invalid data type tag received. Expected \"detected_object_type\", received \""
               << tag << "\". Message dropped." );
  }
  else
  {
    // define our protobuf
    kwiver::protobuf::detected_object_type proto_dot;
    if ( ! proto_dot.ParseFromIstream( &msg ) )
    {
      // throw something
    }

    convert_protobuf( proto_dot, dot );
  }

  deserialize_result_t res;
  res[ DEFAULT_ELEMENT_NAME ] = kwiver::vital::any(dot);

  return res;
}

// ----------------------------------------------------------------------------
bool detected_object_type::
convert_protobuf( const kwiver::protobuf::detected_object_type&  proto_dot,
                  kwiver::vital::detected_object_type& dot )
 {
   const size_t count( proto_dot.name_size() );
   for (size_t i = 0; i < count; ++i )
   {
     dot.set_score( proto_dot.name(i), proto_dot.score(i) );
   }

    return true;
 }

// ----------------------------------------------------------------------------
bool detected_object_type::
convert_protobuf( const kwiver::vital::detected_object_type& dot,
                  kwiver::protobuf::detected_object_type&  proto_dot )
{
  for ( auto it: dot )
  {
    proto_dot.add_name( *(it.first) );
    proto_dot.add_score( it.second);
  }

  return true;
}

} } } } // end namespace
