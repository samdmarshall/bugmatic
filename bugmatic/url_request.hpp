//
//  url_request.hpp
//  bugmatic
//
//  Created by Uli Kusterer on 04/07/16.
//  Copyright © 2016 Uli Kusterer. All rights reserved.
//

#ifndef url_request_hpp
#define url_request_hpp

#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>


class url_reply
{
public:
    inline std::string							data() const			{ return mReceivedContent; }
    inline std::string							content_type() const	{ return mReceivedContentType; }
    inline unsigned int							status() const			{ return mReceivedHttpStatus; }
    inline std::vector<std::string>				headers() const			{ return mReceivedHeaders; }
    std::map<std::string,std::string>			header_map() const;
	std::string									link_header_rel( std::string relValue ) const;
	
	static std::pair<std::string,std::string>	header_name_and_value( std::string inHeaderLine );
	static std::string							header_name( std::string inHeaderLine );
	static std::vector<std::string>				header_list_items( std::string inHeaderLine );	// For splitting Link: <url>; rel="next", <url>; rel="last" -style headers into two URL/rel pairs.
	static std::map<std::string,std::string>	list_item_attributes( std::string inHeaderLine, std::string& outFirstItem );	// For parsing Link: <url>; rel="foo" -style header item generated by header_list_items() into URL and rel=foo.
	
protected:
    std::string					mReceivedContent;
    std::string					mReceivedContentType;
	std::vector<std::string>    mReceivedHeaders;
    unsigned int				mReceivedHttpStatus = 0;

    static size_t		append_content( void* ptr, size_t size, size_t nmemb, void* stream );
    static size_t		append_header( void* ptr, size_t size, size_t nmemb, void* stream );

	friend class url_request;
};


class url_request
{
public:
    CURLcode    load( std::string inURL, url_reply& outReply );

	inline void	add_header( std::string inHeader )							{ mHeadersToSend.push_back(inHeader); }
	inline void	add_post_field( std::string inName, std::string inValue )	{ mPostData[inName] = inValue; }
	inline void set_post_body( std::string n )								{ mPostBody = n; }
	inline void	set_user_name( std::string n )								{ mUserName = n; }
	inline void	set_password( std::string n )								{ mPassword = n; }

protected:
	static size_t		read_body_data( void *ptr, size_t size, size_t nmemb, void *userp );

protected:
    CURL*								mCURLHandle = nullptr;
	std::string							mUserName;
	std::string							mPassword;
	std::vector<std::string>			mHeadersToSend;
	std::map<std::string,std::string>	mPostData;
	std::string							mPostBody;
};


#endif /* url_request_hpp */
