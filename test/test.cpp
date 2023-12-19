#include <iostream>
#include <memory>

#include "fs_mini.hpp"

int main()
{
	const auto file = "test.txt";
	if (fs::is_file_exists(file))
		fs::remove_file(file);

	auto wfd = fs::open_file(file,
				 fs_omode::create | fs_omode::read_write,
				 fs_perms::owner_read);

	const std::string a = "Hello world\n";

        fs::write_object(wfd, a.c_str(), a.size());
	fs::close_file(wfd);

	auto rfd = fs::open_file(file, fs_omode::readonly);
	const auto sz = fs::file_size(rfd);

	std::unique_ptr<char> p(new char[sz]);
	fs::read_object(rfd, p.get(), sz);
	fs::close_file(rfd);

	std::cout << p.get();
}
