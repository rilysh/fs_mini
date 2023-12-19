// Copyright (c) 2023, rilysh <nightquick@proton.me>
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#ifndef FS_MINI_HPP
#define FS_MINI_HPP

// If you don't want to include standard iostream here.
// Note: It's a mandotary header, and if you add an
// alternative or lightweight version of iostream,
// APIs and their behavior must needs to be compatible
// with the standard version of iostream.
#ifndef NO_IOSTREAM_FS_MINI
#  include <iostream>
#endif

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

namespace fs_error {

/**
 *  @breif   Wrapper function that calls std::system_error().
 *  @return  A throwable std::system_error.
 */
const std::system_error get(const std::string& err)
{
	const auto eno = errno;
        return std::system_error(eno, std::generic_category(), err);
}

};

/**
 *  @breif File open modes.
 *
 *
 *  These constants are derived from C header.
 */
namespace fs_omode {

constexpr auto append        = O_APPEND;
constexpr auto async         = O_ASYNC;
constexpr auto close_exec    = O_CLOEXEC;
constexpr auto create        = O_CREAT;
constexpr auto direct        = O_DIRECT;
constexpr auto directory     = O_DIRECTORY;
constexpr auto dsync         = O_DSYNC;
constexpr auto excl          = O_EXCL;
constexpr auto noctty        = O_NOCTTY;
constexpr auto nofollow      = O_NOFOLLOW;
constexpr auto ndelay        = O_NDELAY;
constexpr auto nonblock      = O_NONBLOCK;
constexpr auto path          = O_PATH;
constexpr auto readonly      = O_RDONLY;
constexpr auto read_write    = O_RDWR;
constexpr auto sync          = O_SYNC;
constexpr auto truncate      = O_TRUNC;
constexpr auto writeonly     = O_WRONLY;

#ifdef __linux__
constexpr auto largefile     = O_LARGEFILE;
constexpr auto noatime       = O_NOATIME;
constexpr auto tmpfile       = O_TMPFILE;
#endif

#ifdef __FreeBSD__
constexpr auto execute_only  = O_EXEC;
constexpr auto exclusv_lock  = O_EXLOCK;
constexpr auto shared_lock   = O_SHLOCK;
#endif

};

/**
 *  @breif List of permissions bits
 *
 *  These constants are derived from C header.
 */
namespace fs_perms {

constexpr auto none          = 0;

constexpr auto owner_read    = S_IRUSR;
constexpr auto owner_write   = S_IWUSR;
constexpr auto owner_exec    = S_IXUSR;
constexpr auto owner_all     = S_IRWXU;

constexpr auto group_read    = S_IRGRP;
constexpr auto group_write   = S_IWGRP;
constexpr auto group_exec    = S_IXGRP;
constexpr auto group_all     = S_IRWXG;

constexpr auto others_read   = S_IROTH;
constexpr auto others_write  = S_IWOTH;
constexpr auto others_exec   = S_IXOTH;
constexpr auto others_all    = S_IRWXO;

constexpr auto all           = S_IRWXU | S_IRWXG | S_IRWXO;
constexpr auto set_uid       = S_ISUID;
constexpr auto set_gid       = S_ISGID;
constexpr auto sticky_bit    = S_ISVTX;
constexpr auto mask          = S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX;
constexpr auto unknown       = 0xffff;

};

namespace fs {

/**
 *  @breif  Open a file descriptor.
 *  @return If successful, open() returns the file descriptor.
 */
[[nodiscard]]
int open_file(const std::string& file_path, int flags)
{
	auto fd = ::open(file_path.c_str(), flags);
	if (fd == -1)
		throw fs_error::get("open()");
	
	return fd;
}

/**
 *  @breif  Open a file descriptor.
 *  @return If successful, open() returns the file descriptor.
 */
[[nodiscard]]
int open_file(const std::string& file_path, int flags, mode_t mode)
{
	auto fd = ::open(file_path.c_str(), flags, mode);
	if (fd == -1)
		throw fs_error::get("open()");
	
	return fd;
}

/**
 *  @breif  Close an opened file descriptor.
 *  @return None.
 */
inline void close_file(int fd)
{
	if (::close(fd) == -1)
		throw fs_error::get("close()");
}

/**
 *  @breif  Read data from an opened file descriptor.
 *  @return If successful, read() returns the size of the data,
 *  it read in bytes.
 */
template <typename T>
ssize_t read_object(int fd, T* ptr, std::size_t nbytes)
{
	auto sz = ::read(fd, ptr, nbytes);
        if (sz == -1)
		throw fs_error::get("read()");

	return sz;
}

/**
 *  @breif  Write data to an opened file descriptor.
 *  @return If successful, write() returns the size of the data,
 *  it wrote in bytes.
 */
template <typename T>
ssize_t write_object(int fd, const T* ptr, std::size_t nbytes)
{
	auto sz = ::write(fd, ptr, nbytes);
	if (sz == -1)
		throw fs_error::get("write()");

	return sz;
}

/**
 *  @breif  Retrive file size using stat().
 *  @return If successful, stat() returns the size of the file.
 */
[[nodiscard]]
std::intmax_t file_size(const std::string& path)
{
	struct stat st;

	if (::stat(path.c_str(), &st) == -1)
		throw fs_error::get("stat()");

	return static_cast<std::intmax_t>(st.st_size);
}

/**
 *  @breif  Retrive file size using stat().
 *  @return If successful, stat() returns the size of the file.
 */
[[nodiscard]]
std::intmax_t file_size(int fd)
{
	struct stat st;

	if (::fstat(fd, &st) == -1)
		throw fs_error::get("fstat()");

	return static_cast<std::intmax_t>(st.st_size);
}

/**
 *  @breif  Check whether a file or directory exists or not.
 *  @return If successful, this function returns true otherwise false.
 */
[[nodiscard]]
bool is_file_exists(const std::string& path)
{
	struct stat st;

	if (::stat(path.c_str(), &st) == -1) {
		const auto eno = errno;
		if (eno == ENOENT)
			return false;
		else
			throw fs_error::get("stat()");
	}

	return (st.st_mode & S_IFMT) == S_IFREG;
}

/**
 *  @breif  Check whether a file or directory exists or not.
 *  @return If successful, this function returns true otherwise false.
 */
[[nodiscard]]
bool is_directory_exists(const std::string& path)
{
	struct stat st;
	
	if (::stat(path.c_str(), &st) == -1) {
		const auto eno = errno;
		if (eno == ENOENT)
			return false;
		else
			throw fs_error::get("stat()");
	}

	return (st.st_mode & S_IFMT) == S_IFDIR;
}

/**
 *  @breif  Check whether a symlink exists or not.
 *  @return If successful, this function returns true otherwise false.
 */
[[nodiscard]]
bool is_symlink_exists(const std::string& path)
{
	struct stat st;

	if (::lstat(path.c_str(), &st) == -1) {
		const auto eno = errno;
		if (eno == ENOENT)
			return false;
		else
			throw fs_error::get("lstat()");
        }

	return (st.st_mode & S_IFMT) == S_IFLNK;
}

/**
 *  @breif  Copy a file on the filesystem.
 *  @return None.
 */
void copy_file(const std::string& target, const std::string& dest_path)
{
	auto rfd = ::open(target.c_str(), fs_omode::readonly);
	if (rfd == -1)
	        throw fs_error::get("open()");

	auto wfd = ::open(
		dest_path.c_str(), fs_omode::writeonly | fs_omode::create);
	if (wfd == -1)
		throw fs_error::get("open()");

	struct stat st;

	if (::fstat(rfd, &st) != -1)
		throw fs_error::get("fstat()");

        std::size_t len, c_range;

        len = static_cast<std::size_t>(st.st_size);
	do {
		c_range = ::copy_file_range(rfd, nullptr, wfd, nullptr, len, 0);
		if (c_range == -1UL)
			throw fs_error::get("copy_file_range()");
		len -= c_range;
	} while (len > 0);

	::close(rfd);
	::close(wfd);
}

/**
 *  @breif  Create a symlink of a file or directory on the filesystem.
 *  @return None.
 */
void create_symlink(const std::string& target, const std::string& link_path)
{
	auto ret = ::symlink(target.c_str(), link_path.c_str());
        if (ret == -1)
	        throw fs_error::get("symlink()");
}

/**
 *  @breif Create a hardlink of a file or directory on the filesystem.
 *  @return None.
 */
void create_hardlink(const std::string& old_path, const std::string& new_path)
{
	auto ret = ::link(old_path.c_str(), new_path.c_str());
	if (ret == -1)
		throw fs_error::get("link()");
}

/**
 *  @breif  Get inode protection bits of a file.
 *  @return Protection bits (mode_t).
 */
[[nodiscard]]
mode_t get_permissions(const std::string& file)
{
	struct stat st;
	
	if (::stat(file.c_str(), &st) == -1)
		throw fs_error::get("stat()");

	return st.st_mode;		
}

/**
 *  @breif  Remove or delete a file from the filesystem.
 *  @return None.
 */
void remove_file(const std::string& file)
{
        if (::unlink(file.c_str()) == -1)
		throw fs_error::get("unlink()");
}

/**
 *  @breif  Remove or delete empty directory from the filesystem.
 *  @return None.
 */
void remove_empty_directory(const std::string& dir)
{
	if (::remove(dir.c_str()) == -1)
		throw fs_error::get("remove()");
}

/**
 *  @breif  Create a directory.
 *  @return None.
 */
void create_directory(const std::string& dir, mode_t mode)
{
	if (::mkdir(dir.c_str(), mode) == -1)
		throw fs_error::get("mkdir()");
}

/**
 *  @breif  Change the name or location of a file or a directory.
 *  @return None.
 */
void rename_path(const std::string& old_path, const std::string& new_path)
{
	if (::rename(old_path.c_str(), new_path.c_str()) == -1)
		throw fs_error::get("rename()");
}

/**
 *  @breif  Check whether an environment variable key exists or not.
 *  @return If successful, this function returns true, otherwise false.
 */
[[nodiscard]]
inline bool is_env_exists(const std::string& key)
{ return ::getenv(key.c_str()) != nullptr; }

/**
 *  @breif  Get current working directory.
 *  @return If successful, getenv() returns the value of an environment variable.
 */
[[nodiscard]]
char *current_directory()
{
	const auto ret = ::getenv("PWD");
        if (ret == nullptr)
		throw fs_error::get("getenv()");

	return ret;
}

/**
 *  @breif  Get the environment variable value.
 *  @return If successful, getenv() returns the value of the specified key.
 */
[[nodiscard]]
char *read_env(const std::string& key)
{
	const auto ret = ::getenv(key.c_str());
	if (ret == nullptr)
		throw fs_error::get("getenv()");

	return ret;
}

/**
 *  @breif  Check whether a file type matches with the provided one.
 *  @return If successful, it returns true, otherwise false.
 *  @type   Private function (intended)
 */
bool is_file_match(const std::string& file, unsigned int type)
{
	struct stat st;

	if (::lstat(file.c_str(), &st) == -1)
		throw fs_error::get("lstat()");

	return (st.st_mode & S_IFMT) == type;
}

/**
 *  @breif  Check whether a file is a block device or not.
 *  @return If successful, it will return true otherwise false.
 */
[[nodiscard]]
inline bool is_block_file(const std::string& loc)
{ return is_file_match(loc, S_IFBLK); }

/**
 *  @breif  Check whether a file is a character device or not.
 *  @return If successful, it will return true otherwise false.
 */
[[nodiscard]]
inline bool is_character_file(const std::string& loc)
{ return is_file_match(loc, S_IFCHR); }

/**
 *  @breif  Check whether a file is a directory or not.
 *  @return If successful, it will return true otherwise false.
 */
[[nodiscard]]
inline bool is_directory(const std::string& loc)
{ return is_file_match(loc, S_IFDIR); }

/**
 *  @breif  Check whether a file is a FIFO/pipe or not.
 *  @return If successful, it will return true otherwise false.
 */
[[nodiscard]]
inline bool is_fifo(const std::string& loc)
{ return is_file_match(loc, S_IFIFO); }

/**
 *  @breif  Check whether a file is a FIFO/pipe or not.
 *  @return If successful, it will return true otherwise false.
 *
 *  This function itself is an alias of is_fifo() function.
 */
[[nodiscard]]
inline bool is_pipe(const std::string& loc)
{ return is_fifo(loc); }

/**
 *  @breif  Check whether a file is a symbolic link or not.
 *  @return If successful, it will return true otherwise false.
 */
[[nodiscard]]
inline bool is_symlink(const std::string& loc)
{ return is_file_match(loc, S_IFLNK); }

/**
 *  @breif  Check whether a file is a regular file or not.
 *  @return If successful, it will return true otherwise false.
 */
[[nodiscard]]
inline bool is_regular_file(const std::string& loc)
{ return is_file_match(loc, S_IFREG); }

/**
 *  @breif  Check whether a file is a socket or not.
 *  @return If successful, it will return true otherwise false.
 */
[[nodiscard]]
inline bool is_socket(const std::string& loc)
{ return is_file_match(loc, S_IFSOCK); }

/**
 *  @breif  Get a file type.
 *  @return If successful, it will return the type of the specified file.
 */
[[nodiscard]]
unsigned int get_file_type(const std::string& file)
{
        struct stat st;

	if (::lstat(file.c_str(), &st) == -1)
		throw fs_error::get("lstat()");

	return st.st_mode & S_IFMT;
}

};

#endif
