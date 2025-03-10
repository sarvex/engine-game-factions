/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Level 2 and Level 3 source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2009 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

hkgShaderCollection::hkgShaderCollection() 
{

}

inline hkgShaderCollection* hkgShaderCollection::defaultCreateInternal()
{
	return new hkgShaderCollection();
}


inline void hkgShaderCollection::addShaderGrouping( hkgShader* vshader, hkgShader* pshader, hkgShader* gshader )
{
	ShaderSet& ss = m_shaders.expandOne();
	ss.vertexShader = vshader; if (vshader) vshader->reference();
	ss.geomShader =   gshader; if (gshader) gshader->reference();
	ss.pixelShader =  pshader; if (pshader) pshader->reference();
}

inline void hkgShaderCollection::removeShader( hkgShader* shader )
{
	// could be a pixel or vertex shader
	int numShaders = m_shaders.getSize();
	for (int s =0; s < numShaders; ++s)
	{
		if (m_shaders[s].vertexShader == shader) { m_shaders[s].vertexShader->release(); m_shaders[s].vertexShader = HK_NULL; }
		if (m_shaders[s].geomShader == shader)   { m_shaders[s].geomShader->release();   m_shaders[s].geomShader = HK_NULL; }
		if (m_shaders[s].pixelShader == shader)  { m_shaders[s].pixelShader->release();  m_shaders[s].pixelShader = HK_NULL; }
	}
}

inline void hkgShaderCollection::removeShaderSet( int i )
{
	if (m_shaders[i].vertexShader) m_shaders[i].vertexShader->release();
	if (m_shaders[i].geomShader)   m_shaders[i].geomShader->release();
	if (m_shaders[i].pixelShader)  m_shaders[i].pixelShader->release();
	m_shaders.removeAtAndCopy(i);
}


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
