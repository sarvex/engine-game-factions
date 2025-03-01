/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */
#ifndef HKBASE_HKOSTREAM_H
#define HKBASE_HKOSTREAM_H

class hkOstream;
class hkStreamWriter;
class hkString;
class hkMemoryTrack;
template<typename T> class hkArray;

namespace hkOstreamManip
{
		/// Ostream manipulation operator.
	typedef hkOstream& (HK_CALL *function)(hkOstream&);

		/// Output 'endl'
	hkOstream& HK_CALL endl(hkOstream& os);

		/// Flush stream
	hkOstream& HK_CALL flush(hkOstream& os);	
}



/// Text formatted data writer. Provides functionality similar to std::ostream.
/// All the usual operators are provided plus operators
/// for 64 bit integers. printf style output is also
/// supported.
class hkOstream : public hkReferencedObject
{
	public:

		HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_STREAM);

			/// Constructs an hkOstream using the given hkStreamWriter for writing.
		explicit hkOstream(hkStreamWriter* sr);

			/// Create an ostream connected to a file.
			/// The file is truncated.
		explicit hkOstream(const char* filename);

			/// Create an ostream connected to a memory buffer.
			/// The buffer must exist for the lifetime of this object.
			/// If isString, the buffer is guaranteed to be null terminated.
		hkOstream(void* mem, int memSize, hkBool isString=false);

			/// Create an ostream connected to a growable memory buffer.
			/// The buffer must exist for the lifetime of this object.
		explicit hkOstream( hkArray<char>& buf );

			/// Create an ostream connected to a memory track.
			/// The buffer must exist for the lifetime of this object.
		explicit hkOstream( hkMemoryTrack* track );

			/// Destroys the stream.
		~hkOstream();

			/// Checks the error status of the stream.
		hkBool isOk() const;

			/// Outputs a hex address.
		hkOstream& operator<< (const void* p);

			/// Outputs a hkBool.
		hkOstream& operator<< (hkBool b);

			/// Outputs a char.
		hkOstream& operator<< (char c);

			/// Outputs a signed char.
		hkOstream& operator<< (signed char c);

			/// Outputs an unsigned char.
		hkOstream& operator<< (unsigned char c);

			/// Outputs a string.
		hkOstream& operator<< (const char* s);

			/// Outputs a string.
		hkOstream& operator<< (const signed char* s);

			/// Outputs a string.
		hkOstream& operator<< (const unsigned char* s);

			/// Outputs a short.
		hkOstream& operator<< (short s);

			/// Outputs an unsigned short
		hkOstream& operator<< (unsigned short s);

			/// Outputs an int
		hkOstream& operator<< (int i);

			/// Outputs an unsigned int.
		hkOstream& operator<< (unsigned int u);

			/// Outputs a float.
		hkOstream& operator<< (float f);

			/// Outputs a double.
		hkOstream& operator<< (double d);

			/// Outputs a 64 bit int.
		hkOstream& operator<< (hkInt64 i);

			/// Outputs a 64 bit unsigned int.
		hkOstream& operator<< (hkUint64 u);
			
			/// Outputs an hkString.
		hkOstream& operator<< (const hkString& str);

			/// Outputs raw data.
		int write( const char* buf, int nbytes);

			/// Outputs formatted data.
		void printf( const char *fmt, ...);

			/// Flushes printf stream.
		void flush();

			/// Manipulates the stream format.
		hkOstream& operator<< (hkOstreamManip::function f);

			/// Returns the underlying hkStreamWriter used by this hkOstream.
		hkStreamWriter* getStreamWriter();

			/// Set the underlying hkStreamWriter for this hkOstream
		void setStreamWriter(hkStreamWriter* newWriter);
		
	protected:

		hkStreamWriter*	m_writer;
};

typedef hkOstream hkOfstream;

#define hkendl hkOstreamManip::endl
#define hkflush hkOstreamManip::flush

#include <Common/Base/System/Io/OStream/hkOStream.inl>

#endif // HKBASE_HKOSTREAM_H

/*
* Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20090704)
* 
* Confidential Information of Havok.  (C) Copyright 1999-2009
* Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
* Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
* rights, and intellectual property rights in the Havok software remain in
* Havok and/or its suppliers.
* 
* Use of this software for evaluation purposes is subject to and indicates
* acceptance of the End User licence Agreement for this product. A copy of
* the license is included with this software and is also available at www.havok.com/tryhavok.
* 
*/
