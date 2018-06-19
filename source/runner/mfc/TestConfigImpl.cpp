#include "stdafx.h"
#include "TestConfigImpl.h"

#include <Shlwapi.h>

#include <atlpath.h>
#include <strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

TestConfig *
TestConfig::instance()
{
  static TestConfigImpl s_testConfig;
  return &s_testConfig;
}

TestConfigImpl::TestConfigImpl()
{}

BOOL
TestConfigImpl::isFileExist( CString &file_path )
{
  DWORD attributes = INVALID_FILE_ATTRIBUTES;

  WIN32_FIND_DATA dataFind = {0};
  HANDLE hFind = ::FindFirstFile( file_path, &dataFind );
  if ( hFind != INVALID_HANDLE_VALUE )
  {
    attributes = dataFind.dwFileAttributes;
  }
  ::FindClose( hFind );

  if ( INVALID_FILE_ATTRIBUTES == attributes )
  {
    return FALSE;
  }

  return ( attributes & FILE_ATTRIBUTE_DIRECTORY ) ? FALSE : TRUE;
}

void

TestConfigImpl::loadFailedMsgBox( LPTSTR lib_name )
{
  /*
  Retrieve the system error message for the last-error code
  See also：
  https://msdn.microsoft.com/en-us/library/windows/desktop/ms680582(v=vs.85).aspx
  */

  DWORD error = ::GetLastError();

  LPVOID msgBuf = NULL;
  ::FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    error,
    MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
    ( LPTSTR )&msgBuf,
    0, NULL );

  // Display the error message and exit the process
  LPVOID textBuf = ( LPVOID )::LocalAlloc( LMEM_ZEROINIT,
                   ( lstrlen( ( LPCTSTR )msgBuf ) + lstrlen( ( LPCTSTR )lib_name ) + 40 ) * sizeof( TCHAR ) );
  ::StringCchPrintf( ( LPTSTR )textBuf,
                     ::LocalSize( textBuf ) / sizeof( TCHAR ),
                     _T( "%s load failed!!!\r\ncode : %lu\r\ninfo : %s" ),
                     lib_name, error, ( LPTSTR )msgBuf );

  ::MessageBox( AfxGetAppModuleState()->m_pCurrentWinApp->GetMainWnd()->GetSafeHwnd(), ( LPCTSTR )textBuf, _T( "Error" ), MB_OK );

  ::LocalFree( msgBuf );
  ::LocalFree( textBuf );
}

BOOL
TestConfigImpl::load()
{
  CString exePath;
  ::GetModuleFileName( NULL, exePath.GetBuffer( MAX_PATH ), MAX_PATH );
  exePath.ReleaseBuffer();

  CString xmlPath, exeDirPath;
  {
    // 优先加载exe所在文件夹的配置文件
    ATL::CPath path( exePath );
    exeDirPath = exePath.Left( path.FindFileName() );
    ::PathCombine( xmlPath.GetBuffer( MAX_PATH ), exeDirPath, TEST_CONFIG_FILE );
    xmlPath.ReleaseBuffer();
  }

  if ( !isFileExist( xmlPath ) )
  {
    // 再尝试加载上一级文件夹的配置文件
    ATL::CPath path( exeDirPath );
    CString parentDirPath = exePath.Left( path.FindFileName() );
    ::PathCombine( xmlPath.GetBuffer( MAX_PATH ), parentDirPath, TEST_CONFIG_FILE );
    xmlPath.ReleaseBuffer();
  }

  CComPtr<IXMLDOMDocument> xmlDoc;
  VARIANT_BOOL isSuccessful = VARIANT_FALSE;

  CComPtr<IXMLDOMElement> rootElem;
  CComPtr<IXMLDOMNodeList> testList; // List of <test>

  if ( FAILED( xmlDoc.CoCreateInstance( __uuidof( DOMDocument ) ) )
       || FAILED( xmlDoc->put_async( VARIANT_FALSE ) )
       || FAILED( xmlDoc->put_validateOnParse( VARIANT_FALSE ) )
       || FAILED( xmlDoc->put_resolveExternals( VARIANT_FALSE ) )
       || FAILED( xmlDoc->load( CComVariant( xmlPath ), &isSuccessful ) )
       || isSuccessful != VARIANT_TRUE
       || FAILED( xmlDoc->get_documentElement( &rootElem ) )
       || FAILED( rootElem->get_childNodes( &testList ) ) )
  {
    return false;
  }

  {
    CComPtr<IXMLDOMNamedNodeMap> attrNodeMap;
    CComPtr<IXMLDOMNode> attrNode;
    if ( SUCCEEDED( rootElem->get_attributes( &attrNodeMap ) )
         && SUCCEEDED( attrNodeMap->getNamedItem( _T( "title" ), &attrNode ) )
         && attrNode )
    {
      // 根据title的值来设置窗口的标题
      CComVariant title;
      attrNode->get_nodeValue( &title );
      m_title = title.bstrVal;
    }
  }

  long length = 0;
  testList->get_length( &length );

  for ( long i = 0; i < length; ++i )
  {
    CComPtr<IXMLDOMNode> testNode;
    if ( SUCCEEDED( testList->get_item( i, &testNode ) )
         && testNode )
    {
      CComBSTR name;
      testNode->get_nodeName( &name );
      // 找名为test的node
      if ( name == _T( "test" ) )
      {
        CComPtr<IXMLDOMNamedNodeMap> attrNodeMap;
        CComPtr<IXMLDOMNode> attrNode;
        if ( SUCCEEDED( testNode->get_attributes( &attrNodeMap ) )
             && SUCCEEDED( attrNodeMap->getNamedItem( CComBSTR( _T( "libName" ) ), &attrNode ) )
             && attrNode )
        {
          // 根据libName的配置来加载dll
          CComVariant libName;
          attrNode->get_nodeValue( &libName );
          if ( !::CoLoadLibrary( libName.bstrVal, TRUE ) )
          {
            TestConfigImpl::loadFailedMsgBox( libName.bstrVal );
          }
        }
      }
    }
  }

  return true;
}

LPCTSTR
TestConfigImpl::title()
{
  return m_title;
}
