/* -*-mode:c++; c-file-style: "gnu";-*- */
/*
 *  $Id: Cgicc.cpp,v 1.33 2014/01/01 10:14:50 sebdiaz Exp $
 *
 *  Copyright (C) 1996 - 2004 Stephen F. Booth <sbooth@gnu.org>
 *                       2007 Sebastien DIAZ <sebastien.diaz@gmail.com>
 *  Part of the GNU cgicc library, http://www.gnu.org/software/cgicc
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA 
 */

#ifdef __GNUG__
#  pragma implementation
#endif

#include <string>
#include <stdexcept>
#include <climits>
#include <cfloat>

/*
#include <new>
#include <algorithm>
#include <functional>
#include <iterator>


#include "cgicc/CgiUtils.h"
#include "cgicc/Cgicc.h"
*/

// case-insensitive string comparison
// This code based on code from 
// "The C++ Programming Language, Third Edition" by Bjarne Stroustrup
bool 
stringsAreEqual(const std::string& s1, 
		       const std::string& s2)
{
  std::string::const_iterator p1 = s1.begin();
  std::string::const_iterator p2 = s2.begin();
  std::string::const_iterator l1 = s1.end();
  std::string::const_iterator l2 = s2.end();

  while(p1 != l1 && p2 != l2) {
    if(std::toupper(*(p1++)) != std::toupper(*(p2++)))
      return false;
  }

  return (s2.size() == s1.size()) ? true : false;
}

// case-insensitive string comparison
bool 
stringsAreEqual(const std::string& s1, 
		       const std::string& s2,
		       size_t n)
{
  std::string::const_iterator p1 = s1.begin();
  std::string::const_iterator p2 = s2.begin();
  bool good = (n <= s1.length() && n <= s2.length());
  std::string::const_iterator l1 = good ? (s1.begin() + n) : s1.end();
  std::string::const_iterator l2 = good ? (s2.begin() + n) : s2.end();
  while(p1 != l1 && p2 != l2) {
    if(std::toupper(*(p1++)) != std::toupper(*(p2++)))
      return false;
  }  
  return good;
}

std::string
charToHex(char c)
{
  std::string result;
  char first, second;

  first = (c & 0xF0) / 16;
  first += first > 9 ? 'A' - 10 : '0';
  second = c & 0x0F;
  second += second > 9 ? 'A' - 10 : '0';

  result.append(1, first);
  result.append(1, second);
  
  return result;
}

char
hexToChar(char first,
		 char second)
{
  int digit;
  
  digit = (first >= 'A' ? ((first & 0xDF) - 'A') + 10 : (first - '0'));
  digit *= 16;
  digit += (second >= 'A' ? ((second & 0xDF) - 'A') + 10 : (second - '0'));
  return static_cast<char>(digit);
}

std::string
form_urldecode(const std::string& src)
{
  std::string result;
  std::string::const_iterator iter;
  char c;

  for(iter = src.begin(); iter != src.end(); ++iter) {
    switch(*iter) {
    case '+':
      result.append(1, ' ');
      break;
    case '%':
      // Don't assume well-formed input
      if(std::distance(iter, src.end()) >= 2
	 && std::isxdigit(*(iter + 1)) && std::isxdigit(*(iter + 2))) {
	c = *++iter;
	result.append(1, hexToChar(c, *++iter));
      }
      // Just pass the % through untouched
      else {
	result.append(1, '%');
      }
      break;
    
    default:
      result.append(1, *iter);
      break;
    }
  }
  
  return result;
}

// locate data between separators, and return it
std::string
extractBetween(const std::string& data, 
		      const std::string& separator1, 
		      const std::string& separator2)
{
  std::string result;
  std::string::size_type start, limit;
  
  start = data.find(separator1, 0);
  if(std::string::npos != start) {
    start += separator1.length();
    limit = data.find(separator2, start);
    if(std::string::npos != limit)
      result = data.substr(start, limit - start);
  }
  
  return result;
}
  
template <class T> void Process(T);

// ============================================================
// Class MultipartHeader
// ============================================================
namespace cgicc {

  class CgiInput;


  // ============================================================
  // Class FormFile
  // ============================================================
  
  /*! \class FormFile FormFile.h cgicc/FormFile.h
   * \brief Class representing a file submitted via an HTML form.
   *
   * FormFile is an immutable class reprenting a file uploaded via
   * the HTTP file upload mechanism.  If you are going to use file upload
   * in your CGI application, remember to set the ENCTYPE of the form to
   * \c multipart/form-data.
   * \verbatim
   <form method="post" action="http://change_this_path/cgi-bin/upload.cgi" 
   enctype="multipart/form-data">
   \endverbatim
   * \sa FormEntry
   */
  class FormFile
  {
  public:
    
    // ============================================================
    
    /*! \name Constructors and Destructor */
    //@{
    
    /*!
     * \brief Default constructor
     *
     * Shouldn't be used. 
     */
    inline
    FormFile()
    {}
    
    /*!
     * \brief Create a new FormFile.
     *
     * This is usually not called directly, but by Cgicc.
     * \param name The \e name of the form element.
     * \param filename The \e filename of the file on the remote machine.
     * \param dataType The MIME content type of the data, if specified, or 0.
     * \param data The file data.
     */
    FormFile(const std::string& name, 
	     const std::string& filename, 
	     const std::string& dataType, 
	     const std::string& data);
    
    /*!
     * \brief Copy constructor.
     *
     * Sets the name, filename, datatype, and data to those of \c file
     * @param file The FormFile to copy.
     */
    inline
    FormFile(const FormFile& file)
    { operator=(file); }
    
    /*! 
     * \brief Destructor 
     *
     * Delete this FormFile object
     */
    inline
    ~FormFile()
    {}
    //@}
    
    // ============================================================
    
    /*! \name Overloaded Operators */
    //@{
    
    /*!
     * \brief Compare two FormFiles for equality.
     *
     * FormFiles are equal if they have the same filename.
     * @param file The FormFile to compare to this one.
     * @return \c true if the two FormFiles are equal, \c false otherwise.
     */
    bool 
    operator== (const FormFile& file) 			const;
    
    /*!
     * \brief Compare two FormFiles for inequality.
     *
     * FormFiles are equal if they have the same filename.
     * \param file The FormFile to compare to this one.
     * \return \c false if the two FormFiles are equal, \c true otherwise.
     */
    inline bool 
    operator!= (const FormFile& file) 			const
    { return ! operator==(file); }
    
    
    /*!
     * \brief Assign one FormFile to another.
     *
     * Sets the name, filename, datatype, and data to those of \c file
     * \param file The FormFile to copy.
     * \return A reference to this.
     */
    FormFile& 
    operator= (const FormFile& file);
    //@}
    
    // ============================================================
    
    /*! \name Accessor Methods 
     * Information on the uploaded file
     */
    //@{
    
    /*!
     * \brief Write this file data to the specified stream.
     *
     * This is useful for saving uploaded data to disk
     * \param out The ostream to which to write.
     */
    void 
    writeToStream(std::ostream& out) 			const;
    
    /*!
     * \brief Get the name of the form element.
     *
     * The name of the form element is specified in the HTML form that
     * called the CGI application.
     * \return The name of the form element.
     */
    inline std::string
    getName() 						const
    { return fName; }
    
    /*!
     * \brief Get the basename of the file on the remote machine.
     *
     * The filename is stripped of all leading directory information
     * \return The basename of the file on the remote machine.
     */
    inline std::string
    getFilename() 					const
    { return fFilename; }
    
    /*!
     * \brief Get the MIME type of the file data.
     *
     * This will be of the form \c text/plain or \c image/jpeg
     * \return The MIME type of the file data.  
     */
    inline std::string 
    getDataType() 					const
    { return fDataType; }
    
    /*!
     * \brief Get the file data.  
     *
     * This returns the raw file data as a string
     * \return The file data.
     */
    inline std::string 
    getData() 					const
    { return fData; }
    
    /*!
     * \brief Get the length of the file data
     *
     * The length of the file data is usually measured in bytes.
     * \return The length of the file data, in bytes.
     */
    inline std::string::size_type
    getDataLength() 				const
    { return fData.length(); }
    //@}
    
  private:
    std::string 	fName;
    std::string 	fFilename;
    std::string 	fDataType;
    std::string 	fData;
  };

  class FormEntry
  {
  public:
    
    // ============================================================
    
    /*! \name Constructors and Destructor */
    //@{
    
    /*! 
     * \brief Default constructor
     *
     * Shouldn't be used.
     */
    inline
    FormEntry()
    {}
    
    /*!
     * \brief Create a new FormEntry
     *
     * This is usually not called directly, but by Cgicc.
     * \param name The name of the form element
     * \param value The value of the form element
     */
    inline
    FormEntry(const std::string& name, 
	      const std::string& value)
      : fName(name), fValue(value)
    {}
    
    /*!
     * \brief Copy constructor.
     *
     * Sets the name and value of this FormEntry to those of \c entry.
     * \param entry The FormEntry to copy.
     */
    inline
    FormEntry(const FormEntry& entry)
    { operator=(entry); }
    
    /*! 
     * \brief Destructor.
     *
     * Delete this FormEntry object
     */
    inline
    ~FormEntry()
    {}
    //@}
    
    // ============================================================
    
    /*! \name Overloaded Operators */
    //@{
    
    /*!
     * \brief Compare two FormEntrys for equality.
     *
     * FormEntrys are equal if they have the same name and value.
     * \param entry The FormEntry to compare to this one.
     * \return \c true if the two FormEntrys are equal, \c false otherwise.
     */
    inline bool 
    operator== (const FormEntry& entry) 		const
    { return stringsAreEqual(fName, entry.fName); }
    
    /*!
     * \brief Compare two FormEntrys for inequality.
     *
     * FormEntrys are equal if they have the same name and value.
     * \param entry The FormEntry to compare to this one.
     * \return \c false if the two FormEntrys are equal, \c true otherwise.
     */
    inline bool
    operator!= (const FormEntry& entry) 		const
    { return ! operator==(entry); }
    
    
    /*!
     * \brief Assign one FormEntry to another.  
     *
     * Sets the name and value of this FormEntry to those of \c entry.
     * \param entry The FormEntry to copy.
     * \return A reference to this.
     */
    FormEntry& 
    operator= (const FormEntry& entry);
    //@}
    
    // ============================================================
    
    /*! \name Accessor Methods 
     * Information on the form element
     */
    //@{
    
    /*!
     * \brief Get the name of the form element.
     *
     * The name of the form element is specified in the HTML form that
     * called the CGI application.
     * \return The name of the form element.
     */
    inline std::string
    getName() 						const
    { return fName; }
    
    /*!
     * \brief Get the value of the form element as a string
     *
     * The value returned may contain line breaks.
     * \return The value of the form element.
     */
    inline std::string
    getValue() 						const
    { return fValue; }
    
    /*!
     * \brief Get the value of the form element as a string
     *
     * The value returned may contain line breaks.
     * \return The value of the form element.
     */
    inline std::string
    operator* () 					const
    { return getValue(); }
    
    /*!
     * \brief Get the value of the form element as a string
     *
     * The value returned will be truncated to a specific length.
     * The value may contain line breaks.
     * \param maxChars The maximum number of characters to return.
     * \return The value of the form element, truncated to the specified length.
     */
    inline std::string
    getValue(std::string::size_type maxChars) 		const
    { return makeString(maxChars, true); }
    
    /*!
     * \brief Get the value of the form element as a string
     *
     * The value returned will be stripped of all line breaks.
     * \return The value of the form element, stripped of all line breaks.
     */
    inline std::string
    getStrippedValue() 					const
    { return getStrippedValue(INT_MAX); }
    
    /*!
     * \brief Get the value of the form element as a string
     *
     * The value returned will be stripped of all line breaks
     * and truncated to a specific length.
     * \param maxChars The maximum number of characters to return.
     * \return The value of the form element, stripped of all line breaks and
     * truncated to the specified length.
     */
    inline std::string
    getStrippedValue(std::string::size_type maxChars) 	const
    { return makeString(maxChars, false); }
    
    /*!
     * \brief Get the value of the form element as an integer
     *
     * No syntax checking is performed on the string value.
     * \param min The minimum value to return (optional).
     * \param max The maximum value to return (optional).
     * \return The integer value of the form element.
     */
    long
    getIntegerValue(long min = LONG_MIN, 
		    long max = LONG_MAX) 		const;
    
    /*!
     * \brief Get the value of the form element as an integer
     *
     * No syntax checking is performed on the string value.
     * \param min The minimum value to return.
     * \param max The maximum value to return.
     * \param bounded Set to \c true if the value was originally outside the 
     * limits, \c false otherwise
     * \return The integer value of the form element.
     */
    long
    getIntegerValue(long min, 
		    long max,
		    bool& bounded) 			const;
    
    /*!
     * \brief Get the value of the form element as a double
     *
     * No syntax checking is performed on the string value.
     * \param min The minimum value to return (optional).
     * \param max The maximum value to return (optional).
     * \return The double value of the form element.
     */
    double 
    getDoubleValue(double min = -DBL_MAX, 
		   double max = DBL_MAX) 		const;
    
    /*!
     * \brief Get the value of the form element as a double
     *
     * No syntax checking is performed on the string value.
     * \param min The minimum value to return.
     * \param max The maximum value to return.
     * \param bounded Set to \c true if the value was originally outside the 
     * limits, \c false otherwise
     * \return The double value of the form element.
     */
    double 
    getDoubleValue(double min, 
		   double max,
		   bool& bounded) 			const;
    
    
    /*!
     * \brief Get the number of characters in the value of the form element.
     *
     * Note that a character may or may not equal one byte.
     * \return The length of the value of the form element
     */
    inline std::string::size_type
    length() 						const
    { return fValue.length(); }
    
    /*!
     * \brief Determine if this form element is empty
     *
     * In an empty form element, length() == 0.
     * \return \c true if this form element is empty, \c false otherwise.
     */
    inline bool 
    isEmpty() 						const
    { return (0 == length()); }
    //@}
    
  private:  
    // utility function
    std::string
    makeString(std::string::size_type maxLen, 
	       bool allowNewlines) 			const;
    
    std::string fName;		// the name of this form element
    std::string fValue;		// the value of this form element
  };

  class CgiEnvironment
  {
  public:

    CgiEnvironment(CgiInput *input)
    {
      readEnvironmentVariables(input);

      if(stringsAreEqual(fRequestMethod, "post")) {
        // Don't use auto_ptr, but vector instead
        // Bug reported by shinra@j10n.org
        //std::vector<char> data(fContentLength);
        
        // If input is 0, use the default implementation of CgiInput
	if ( getContentLength() )
          {
            // If input is 0, use the default implementation of CgiInput
            
            //            if ( input->read( &data[0], getContentLength() ) != getContentLength() )
            //	throw std::runtime_error("I/O error");
            
            //fPostData = std::string( &data[0], getContentLength() );
          } 
      }
      
      //fCookies.reserve(10);
      parseCookies();
      
    }

    inline CgiEnvironment(const CgiEnvironment& env)    { operator=(env); }

    ~CgiEnvironment(){}

    bool 
    operator== (const CgiEnvironment& env) 		const
{
  bool result;
  
  result =  fServerPort 	== env.fServerPort;
  result &= fContentLength 	== env.fContentLength;
  result &= fUsingHTTPS 	== env.fUsingHTTPS;
  result &= fServerSoftware 	== env.fServerSoftware;
  result &= fServerName 	== env.fServerName;
  result &= fGatewayInterface 	== env.fGatewayInterface;
  result &= fServerProtocol 	== env.fServerProtocol;
  result &= fRequestMethod 	== env.fRequestMethod;
  result &= fPathInfo 		== env.fPathInfo;
  result &= fPathTranslated 	== env.fPathTranslated;
  result &= fScriptName 	== env.fScriptName;
  result &= fQueryString 	== env.fQueryString;
  result &= fRemoteHost 	== env.fRemoteHost;
  result &= fRemoteAddr 	== env.fRemoteAddr;
  result &= fAuthType 		== env.fAuthType;
  result &= fRemoteUser 	== env.fRemoteUser;
  result &= fRemoteIdent 	== env.fRemoteIdent;
  result &= fContentType 	== env.fContentType;
  result &= fAccept 		== env.fAccept;
  result &= fUserAgent 		== env.fUserAgent;
  result &= fPostData 		== env.fPostData;
  result &= fRedirectRequest 	== env.fRedirectRequest;
  result &= fRedirectURL 	== env.fRedirectURL;
  result &= fRedirectStatus 	== env.fRedirectStatus;
  result &= fReferrer 		== env.fReferrer;
  result &= fCookie 		== env.fCookie;

  return result;
}

    
    inline bool
    operator!= (const CgiEnvironment& env) 		const
    { return ! operator==(env); }
    

    
    /*!
     * \brief Assign one CgiEnvironment to another.  
     *
     * Sets the environment variables in this CgiEnvironment to those of \c env.
     * \param env The CgiEnvironment to copy.
     * \return A reference to this.
     */
    CgiEnvironment& 
    operator= (const CgiEnvironment& env)
{
  fServerPort 		= env.fServerPort;
  fContentLength 	= env.fContentLength;
  fUsingHTTPS 		= env.fUsingHTTPS;
  fServerSoftware 	= env.fServerSoftware;
  fServerName 		= env.fServerName;
  fGatewayInterface 	= env.fGatewayInterface;
  fServerProtocol 	= env.fServerProtocol;
  fRequestMethod 	= env.fRequestMethod;
  fPathInfo 		= env.fPathInfo;
  fPathTranslated 	= env.fPathTranslated;
  fScriptName 		= env.fScriptName;
  fQueryString 		= env.fQueryString;
  fRemoteHost 		= env.fRemoteHost;
  fRemoteAddr 		= env.fRemoteAddr;
  fAuthType 		= env.fAuthType;
  fRemoteUser 		= env.fRemoteUser;
  fRemoteIdent 		= env.fRemoteIdent;
  fContentType 		= env.fContentType;
  fAccept 		= env.fAccept;
  fUserAgent 		= env.fUserAgent;
  fPostData 		= env.fPostData;
  fRedirectRequest 	= env.fRedirectRequest;
  fRedirectURL 		= env.fRedirectURL;
  fRedirectStatus 	= env.fRedirectStatus;
  fReferrer 		= env.fReferrer;
  fCookie 		= env.fCookie;

  //fCookies.clear();
  //  fCookies.reserve(env.fCookies.size());
  parseCookies();

  return *this;
}


    inline std::string 
    getServerSoftware() 			const
    { return fServerSoftware; }
    
    /*!
     * \brief Get the hostname, DNS name or IP address of the HTTP server
     *
     * This is \e not a URL, for example \c www.gnu.org (no leading http://)
     * \return The name of the server
     */
    inline std::string 
    getServerName() 				const
    { return fServerName; }
    
    /*!
     * \brief Get the name and version of the gateway interface.
     *
     * This is usually \c CGI/1.1
     * \return The name and version of the gateway interface
     */
    inline std::string 
    getGatewayInterface() 			const
    { return fGatewayInterface;}
    
    /*!
     * \brief Get the name and revision of the protocol used for this request.
     *
     * This is usually \c HTTP/1.0 or \c HTTP/1.1
     * \return The protocol in use
     */
    inline std::string 
    getServerProtocol() 			const
    { return fServerProtocol; }
    
    /*!
     * \brief Get the port number on the server to which this request was sent.
     *
     * This will usually be 80.
     * \return The port number
     */
    inline unsigned long
    getServerPort() 				const
    { return fServerPort; }
    
    /*!
     * \brief Determine if this is a secure request
     * 
     * A secure request is usually made using SSL via HTTPS
     * \return \c true if this connection is via https, \c false otherwise
     */
    inline bool
    usingHTTPS() 				const
    { return fUsingHTTPS; }
    //@}
    
    // ============================================================
    
    /*! \name CGI Query Information
     * Information specific to this CGI query
     */
    //@{
    
    /*!
     * \brief Get the HTTP cookies associated with this query, if any.  
     *
     * The string returned by this method may contain multiple cookies; it is
     * recommended to use the method getCookieList() instead, which returns
     * a \c vector<HTTPCookie>.
     * \return The HTTP cookies
     * \see getCookieList
     */
    inline std::string 
    getCookies() 				const
    { return fCookie; }
    
    /*!
     * \brief Get a \c vector containing the HTTP cookies 
     * associated with this query
     *
     * This vector may be empty
     * \return A \c vector containing the HTTP cookies associated with this
     *  query
     * \see HTTPCookie
     */
    //    inline const std::vector<HTTPCookie>&     getCookieList() 				const    { return fCookies; }
    
    /*!
     * \brief Get the request method used for this query.
     *
     * This is usually one of \c GET or \c POST
     * \return The request method
     */
    inline std::string 
    getRequestMethod()   			const
    { return fRequestMethod; }
    
    /*!
     * \brief Get the extra path information for this request, given by the 
     * client.
     *
     * For example, in the string \c foo.cgi/cgicc the path information is
     * \c cgicc.
     * \return The absolute path info
     */
    inline std::string
    getPathInfo() 				const
    { return fPathInfo; }
    
    /*!
     * \brief Get the translated path information (virtual to physical mapping).
     *
     * For example, \c www.gnu.org may be translated to \c /htdocs/index.html
     * \return The translated path info
     */
    inline std::string 
    getPathTranslated() 			const
    { return fPathTranslated; }
    
    /*!
     * \brief Get the full path to this CGI application
     *
     * This is useful for self-referencing URIs
     * \return The full path of this application
     */
    inline std::string 
    getScriptName() 				const
    { return fScriptName; }
    
    /*!
     * \brief Get the query string for this request.
     *
     * The query string follows the <tt>?</tt> in the URI which called this
     * application. This is usually only valid for scripts called with 
     * the \c GET method. For example, in the string \c foo.cgi?cgicc=yes 
     * the query string is \c cgicc=yes.
     * @return The query string
     */
    inline std::string 
    getQueryString()  				const
    { return fQueryString; }
    
    /*!
     * \brief Get the length of the data read from standard input, in chars.
     *
     * This is usually only valid for scripts called with the POST method.
     * \return The data length
     */
    inline unsigned long
    getContentLength() 				const
    { return fContentLength; }
    
    /*!
     * \brief Get the content type of the submitted information.
     *
     * For applications called via the GET method, this information is
     * irrelevant.  For applications called with the POST method, this is
     * specifies the MIME type of the information, 
     * usually \c application/x-www-form-urlencoded or as specified by
     * getContentType().
     * \return The content type
     * \see getContentType
     */
    inline std::string 
    getContentType() 				const
    { return fContentType; }
    
    /*!
     * \brief Get the data passed to the CGI application via standard input.
     *
     * This data is of MIME type \c getContentType().
     * \return The post data.
     */
    inline std::string
    getPostData() 				const
    { return fPostData; }
    //@}
    
    // ============================================================
    
    /*! \name Server Specific Information
     * Information dependent on the type of HTTP server in use
     */
    //@{
    
    /*!
     * \brief Get the URL of the page which called this CGI application.
     *
     * Depending on the HTTP server software, this value may not be set.
     * \return The URI which called this application.
     */
    inline std::string 
    getReferrer() 				const
    { return fReferrer; }
    //@}
    
    // ============================================================
    
    /*! \name Remote User Information
     * Information about the user making the CGI request
     */
    //@{
    
    /*!
     * \brief Get the hostname of the remote machine making this request
     *
     * This may be either an IP address or a hostname
     * \return The remote host
     */
    inline std::string 
    getRemoteHost() 				const
    { return fRemoteHost; }
    
    /*!
     * \brief Get the IP address of the remote machine making this request
     *
     * This is a standard IP address of the form \c 123.123.123.123
     * \return The remote IP address
     */
    inline std::string 
    getRemoteAddr() 				const
    { return fRemoteAddr; }
    
    /*!
     * \brief Get the protocol-specific user authentication method used.
     *
     * This is only applicable if the server supports user authentication,
     * and the user has authenticated.
     * \return The authorization type
     */
    inline std::string 
    getAuthType() 				const
    { return fAuthType; }
    
    /*!
     * \brief Get the authenticated remote user name.
     *
     * This is only applicable if the server supports user authentication,
     * and the user has authenticated.
     * \return The remote username 
     */
    inline std::string 
    getRemoteUser() 				const
    { return fRemoteUser; }
    
    /*!
     * \brief Get the remote user name retrieved from the server.
     *
     * This is only applicable if the server supports RFC 931 
     * identification.  This variable should \e only be used
     * for logging purposes.
     * \return The remote identification
     * \see RFC 1431 at 
     * http://info.internet.isi.edu:80/in-notes/rfc/files/rfc1413.txt
     */
    inline std::string 
    getRemoteIdent()    			const
    { return fRemoteIdent; }
    
    /*!
     * \brief Get the MIME data types accepted by the client's browser.
     *
     * For example <TT>image/gif, image/x-xbitmap, image/jpeg, image/pjpeg</TT>
     * \return The accepted data types
     */
    inline std::string 
    getAccept() 				const
    { return fAccept; }
    
    /*!
     * \brief Get the name of the browser used for this CGI request.
     *
     * For example <TT>Mozilla/5.0 (X11; U; Linux 2.4.0 i686; en-US; 0.8.1) 
     * Gecko/20010421</TT>
     * \return The browser name
     */
    inline std::string 
    getUserAgent() 				const
    { return fUserAgent; }
    //@}
    
    // ============================================================
    
    /*! \name ErrorDocument Handling
     * For a tutorial on ErrorDocument handling, see 
     * http://hoohoo.ncsa.uiuc.edu/cgi/ErrorCGI.html
     */
    //@{
    
    /*!
     * \brief Get the redirect request.
     *
     * This will only be valid if you are using this script as a script
     * to use in place of the default server messages.
     * @return The redirect request.
     */
    inline std::string 
    getRedirectRequest() 			const
    { return fRedirectRequest; }
    
    /*!
     * \brief Get the redirect URL.
     *
     * This will only be valid if you are using this script as a script
     * to use in place of the default server messages.
     * \return The redirect URL.
     * \see http://hoohoo.ncsa.uiuc.edu/docs/setup/srm/ErrorDocument.html
     */
    inline std::string 
    getRedirectURL() 				const
    { return fRedirectURL; }
    
    /*!
     * \brief Get the redirect status.
     *
     * This will only be valid if you are using this script as a script
     * to use in place of the default server messages.
     * \return The redirect status.
     */
    inline std::string 
    getRedirectStatus() 			const
    { return fRedirectStatus; }
    //@}
    
    void     save(const std::string& filename) 		const;
    void     restore(const std::string& filename);
    //@}
    
    // ============================================================
    
  private:
    
    // Parse the list of cookies from a string to a vector
    void 
    parseCookies();
    
    // Parse a single cookie string (name=value) pair
    void
    parseCookie(const std::string& data);
    
    // Read in all the environment variables
    void 
    readEnvironmentVariables(CgiInput *input);
    
    unsigned long 		fServerPort;
    unsigned long 		fContentLength;
    bool			fUsingHTTPS;
    std::string 		fServerSoftware;
    std::string 		fServerName;
    std::string 		fGatewayInterface;
    std::string 		fServerProtocol;
    std::string 		fRequestMethod;
    std::string 		fPathInfo;
    std::string 		fPathTranslated;
    std::string 		fScriptName;
    std::string 		fQueryString;
    std::string 		fRemoteHost;
    std::string 		fRemoteAddr;
    std::string 		fAuthType;
    std::string 		fRemoteUser;
    std::string 		fRemoteIdent;
    std::string 		fContentType;
    std::string 		fAccept;
    std::string 		fUserAgent;
    std::string 		fPostData;
    std::string 		fRedirectRequest;
    std::string 		fRedirectURL;
    std::string 		fRedirectStatus;
    std::string 		fReferrer;
    std::string 		fCookie;
    //    std::vector<HTTPCookie> 	fCookies;
    std::string                 fAcceptLanguageString; 
  };
  

class MultipartHeader 
{
public:

  MultipartHeader(const std::string& disposition,
					const std::string& name,
					const std::string& filename,
					const std::string& cType)
  : fContentDisposition(disposition),
    fName(name),
    fFilename(filename),
    fContentType(cType)
  {}

  
  inline
  MultipartHeader(const MultipartHeader& head)
  { operator=(head); }

  ~MultipartHeader(){}

  MultipartHeader&
  operator= (const MultipartHeader& head)
  {
    fContentDisposition 	= head.fContentDisposition;
    fName 		= head.fName;
    fFilename 		= head.fFilename;
    fContentType 		= head.fContentType;    
    return *this;
  }
  
  inline std::string 
  getContentDisposition() 				const
  { return fContentDisposition; }
  
  inline std::string
  getName() 						const
  { return fName; }

  inline std::string 
  getFilename() 					const
  { return fFilename; }

  inline std::string 
  getContentType() 					const
  { return fContentType; }

private:
  std::string fContentDisposition;
  std::string fName;
  std::string fFilename;
  std::string fContentType;
};




  class  Cgicc {
  public:
    
    inline
    Cgicc(const Cgicc& cgi)
      : fEnvironment(cgi.fEnvironment)
    { operator=(cgi); }

    inline bool 
    operator== (const Cgicc& cgi) 		const
    { return this->fEnvironment == cgi.fEnvironment; }
    
    inline bool
    operator!= (const Cgicc& cgi) 		const
    { return ! operator==(cgi); }
    
    
    bool 
    queryCheckbox(const std::string& elementName) 	const;
    
    std::string
    operator() (const std::string& name) 		const;
    
    inline const CgiEnvironment&
    getEnvironment() 					const
    { return fEnvironment;}
    

    
  private:
    CgiEnvironment 		fEnvironment;
    //    std::vector<FormEntry> 	fFormData;
    //std::vector<FormFile> 	fFormFiles;

    // Convert query string into a list of FormEntries

    
    // Parse a multipart/form-data header
    //    MultipartHeader    parseHeader(const std::string& data);
    
    // Parse a (name=value) form entry
    void 
    parsePair(const std::string& data);
    
    // Parse a MIME entry for ENCTYPE=""
    //    void    parseMIME(const std::string& data);
    
    // Find elements in the list of entries
    //    bool 
    //    findEntries(const std::string& param, 
    //		bool byName,
                //		std::vector<FormEntry>& result) 	const;

    Cgicc(CgiInput *input)
      : fEnvironment(input)
    { 
      // this can be tweaked for performance
      //fFormData.reserve(20);
      //fFormFiles.reserve(2);
      
      parseFormInput(fEnvironment.getPostData(), fEnvironment.getContentType());
      parseFormInput(fEnvironment.getQueryString());
    }
    
    ~Cgicc()
    {}
    
    Cgicc& 
    operator= (const Cgicc& cgi)
    {
      this->fEnvironment = cgi.fEnvironment;

      //fFormData.clear();
      //fFormFiles.clear();

  parseFormInput(fEnvironment.getPostData(), fEnvironment.getContentType());
  parseFormInput(fEnvironment.getQueryString());
  
  return *this;
}

const char* getCompileDate() const { return __DATE__; }

const char*
getCompileTime() 					const
{ return __TIME__; }

const char*
getVersion() 					const
{ return "unstable test version"; }

const char*
getHost() 					const
{ return "dev server"; }

void
save(const std::string& filename) 		const
{
  fEnvironment.save(filename);
}

void restore(const std::string& filename) {
  fEnvironment.restore(filename);

  // clear the current data and re-parse the enviroment
  //  fFormData.clear();
  //  fFormFiles.clear();

  parseFormInput(fEnvironment.getPostData(), fEnvironment.getContentType());
  parseFormInput(fEnvironment.getQueryString());
}


    //void         parseFormInput(const std::string& data, const std::string& content_type = );
void
parseFormInput(const std::string& data, 
               const std::string &content_type = "application/x-www-form-urlencoded")
{
  
  std::string standard_type	= "application/x-www-form-urlencoded";
  std::string multipart_type 	= "multipart/form-data";

  // Don't waste time on empty input
  if(true == data.empty())
    return;

  // Standard content type = application/x-www-form-urlencoded
  // It may not be explicitly specified
  if(true == content_type.empty() 
     || stringsAreEqual(content_type, standard_type,standard_type.length())) {
    std::string name, value;
    std::string::size_type pos;
    std::string::size_type oldPos = 0;

    // Parse the data in one fell swoop for efficiency
    while(true) {
      // Find the '=' separating the name from its value, also have to check for '&' as its a common misplaced delimiter but is a delimiter none the less
      pos = data.find_first_of( "&=", oldPos);
      
      // If no '=', we're finished
      if(std::string::npos == pos)
	break;
      
      // Decode the name
	// pos == '&', that means whatever is in name is the only name/value
      if( data.at( pos ) == '&' )
	  {
	  	const char * pszData = data.c_str() + oldPos;
		while( *pszData == '&' ) // eat up extraneous '&'
		{
			++pszData; ++oldPos;
		}
		if( oldPos >= pos )
		{ // its all &'s
			oldPos = ++pos;
			continue;
		}
		// this becomes an name with an empty value
		name = form_urldecode(data.substr(oldPos, pos - oldPos));
		Process(FormEntry(name, "" ) );
		oldPos = ++pos;
		continue;
	  }
      name = form_urldecode(data.substr(oldPos, pos - oldPos));
      oldPos = ++pos;
      
      // Find the '&' or ';' separating subsequent name/value pairs
      pos = data.find_first_of(";&", oldPos);
      
      // Even if an '&' wasn't found the rest of the string is a value
      value = form_urldecode(data.substr(oldPos, pos - oldPos));

      // Store the pair
      Process(FormEntry(name, value));
      
      if(std::string::npos == pos)
	break;

      // Update parse position
      oldPos = ++pos;
    }
  }
  // File upload type = multipart/form-data
  else if(stringsAreEqual(multipart_type, content_type,
			  multipart_type.length())){

    // Find out what the separator is
    std::string 		bType 	= "boundary=";
    std::string::size_type 	pos 	= content_type.find(bType);

    // Remove next sentence
    std::string                 commatek=";";

    // generate the separators
    std::string sep1 = content_type.substr(pos + bType.length());
    if (sep1.find(";")!=std::string::npos)
       sep1=sep1.substr(0,sep1.find(";"));
    sep1.append("\r\n");
    sep1.insert(0, "--");

    std::string sep2 = content_type.substr(pos + bType.length());
    if (sep2.find(";")!=std::string::npos)
       sep2=sep2.substr(0,sep2.find(";"));
    sep2.append("--\r\n");
    sep2.insert(0, "--");

    // Find the data between the separators
    std::string::size_type start  = data.find(sep1);
    std::string::size_type sepLen = sep1.length();
    std::string::size_type oldPos = start + sepLen;

    while(true) {
      pos = data.find(sep1, oldPos);

      // If sep1 wasn't found, the rest of the data is an item
      if(std::string::npos == pos)
	break;

      // parse the data
      parseMIME(data.substr(oldPos, pos - oldPos));

      // update position
      oldPos = pos + sepLen;
    }

    // The data is terminated by sep2
    pos = data.find(sep2, oldPos);
    // parse the data, if found
    if(std::string::npos != pos) {
      parseMIME(data.substr(oldPos, pos - oldPos));
    }
  }
}

cgicc::MultipartHeader
parseHeader(const std::string& data)
{
  std::string disposition;
  disposition = extractBetween(data, "Content-Disposition: ", ";");
  
  std::string name;
  name = extractBetween(data, "name=\"", "\"");
  
  std::string filename;
  filename = extractBetween(data, "filename=\"", "\"");

  std::string cType;
  cType = extractBetween(data, "Content-Type: ", "\r\n\r\n");

  // This is hairy: Netscape and IE don't encode the filenames
  // The RFC says they should be encoded, so I will assume they are.
  filename = form_urldecode(filename);

  return MultipartHeader(disposition, name, filename, cType);
}

void parseMIME(const std::string& data)
{
  // Find the header
  std::string end = "\r\n\r\n";
  std::string::size_type headLimit = data.find(end, 0);
  
  // Detect error
  if(std::string::npos == headLimit)
    throw std::runtime_error("Malformed input");

  // Extract the value - there is still a trailing CR/LF to be subtracted off
  std::string::size_type valueStart = headLimit + end.length();
  std::string value = data.substr(valueStart, data.length() - valueStart - 2);

  // Parse the header - pass trailing CR/LF x 2 to parseHeader
  MultipartHeader head = parseHeader(data.substr(0, valueStart));

  if(head.getFilename().empty())
    Process(FormEntry(head.getName(), value));
  else
    Process(
            FormFile(
                     head.getName(), 
                     head.getFilename(), 
                     head.getContentType(), 
                     value));
}
  };

}
