#include "precompiled.h"

#include "BadArchiveFactory.h"
#include "BadArchive.h"

#include "Logging/Logger.h"
using namespace Logging;

#include "Exceptions/NullReferenceException.hpp"

namespace Renderer
{
  static const std::string BAD_ARCHTYPE = "BAD";

  const Ogre::String& BadArchiveFactory::getType() const
  {
    return BAD_ARCHTYPE;
  }

  Ogre::Archive* BadArchiveFactory::createInstance(const Ogre::String& name)
  {
    Debug(name);

    std::string archiveName = "";

    int lastIndexOfPath = name.rfind('/');
    if(lastIndexOfPath != name.npos)
    {
      archiveName = name.substr(lastIndexOfPath + 1, name.length() - lastIndexOfPath);
    }

    int extensionIndex = archiveName.find('.');
    if (extensionIndex != archiveName.npos)
    {
      archiveName.replace(extensionIndex, archiveName.length() - extensionIndex, "");
    }

    return new BadArchive(archiveName, BAD_ARCHTYPE, m_resourceCache);
  }

  void BadArchiveFactory::destroyInstance(Ogre::Archive* archive)
  {
    if (0 == archive)
    {
      NullReferenceException e("BadArchiveFactory::destroyInstance -- Archive is NULL");
      Fatal(e.what());
      throw e;
    }

    Debug(archive->getName());

    delete archive;
  }
}