//
//  fake_filesystem.cpp
//  bugmatic
//
//  Created by Uli Kusterer on 19/11/2016.
//  Copyright © 2016 Uli Kusterer. All rights reserved.
//

#include "fake_filesystem.hpp"
#include <dirent.h>


using namespace fake::filesystem;


std::ostream& fake::filesystem::operator << ( std::ostream& inOutputStream, const path& inPath )
{
	return inOutputStream << inPath.string();
}


path path::filename() const
{
	off_t searchStart = std::string::npos;
	if( mPath.length() > 0 && mPath[mPath.length()-1] == '/' )	// Ends in slash?
		searchStart = mPath.length() -2;	// Ignore trailing slash.
	off_t pos = mPath.rfind("/", searchStart);
    if( pos != std::string::npos )
        return path(mPath.substr(pos +1));
	return "";
}


path path::parent_path() const
{
	off_t searchStart = std::string::npos;
	if( mPath.length() > 0 && mPath[mPath.length()-1] == '/' )	// Ends in slash?
		searchStart = mPath.length() -2;	// Ignore trailing slash.
	off_t pos = mPath.rfind("/",searchStart);
    if( pos != std::string::npos )
	{
        return path(mPath.substr(0,pos));
	}
	return "";
}


path& path::operator /= ( const path& inAppendee )
{
	if( mPath.length() > 0 && mPath[mPath.length()-1] != '/' )
		mPath.append("/");
	mPath.append(inAppendee.mPath);
	
	return *this;
}


path path::operator / ( const path& inAppendee ) const
{
	path	newPath(*this);
	if( mPath.length() > 0 && mPath[mPath.length()-1] != '/' )
		newPath.mPath.append("/");
	newPath.mPath.append(inAppendee.mPath);
	
	return newPath;
}


directory_iterator::directory_iterator( path inPath )
{
	mPath = inPath;
    mDir = new dir(opendir( inPath.string().c_str() ));
	
	// Fetch the first file entry:
	++(*this);
}


directory_iterator::directory_iterator( const directory_iterator& inOriginal )
{
	mPath = inOriginal.mPath;
	mDir = inOriginal.mDir->acquire();
}


directory_iterator::~directory_iterator()
{
	if( mDir )
		mDir->release();
}


directory_iterator& directory_iterator::operator =( const directory_iterator& inOriginal )
{
	mPath = inOriginal.mPath;
	if( mDir != inOriginal.mDir )
	{
		if( mDir )
			mDir->release();
		if( inOriginal.mDir )
			mDir = inOriginal.mDir->acquire();
		else
			mDir = nullptr;
	}
	return *this;
}

directory_iterator directory_iterator::operator ++ ()
{
	if( !mDir || mDir->get_dir() == nullptr )
	{
		mEntry.mPath = "";
		return *this;
	}
	
    struct dirent *dp = nullptr;
	do
	{
		dp = readdir( mDir->get_dir() );
		if( dp )
			mEntry.mPath = mPath / dp->d_name;
	}
	while( dp && (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) );

	if( dp == nullptr )
		mEntry.mPath = "";
	
	return *this;
}


bool	directory_iterator::operator == ( const directory_iterator& inOther ) const
{
	return inOther.mEntry.mPath == mEntry.mPath;
}

bool	directory_iterator::operator != ( const directory_iterator& inOther ) const
{
	return inOther.mEntry.mPath != mEntry.mPath;
}
