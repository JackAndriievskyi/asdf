#include "Avm.h"
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>

static std::string	generateTmpFileName()
{
	time_t     now = time(0);

	struct tm  tstruct;
	localtime_s(&tstruct, &now);

	char       buf[128];
	strftime(buf, sizeof(buf), "%Y_%m_%d_%X", &tstruct);

	return std::string("/tmp/AVM_") + std::string(buf) + std::string(".avm");
} 

static bool			makeTmpFile(const std::string & tmp_file_name)
{
	std::ofstream	tmp_file(tmp_file_name,
								std::ios_base::out | std::ios_base::app);
	std::cout << tmp_file_name;
	if (!tmp_file.is_open()) {
		std::cout << "Error: Oooooops! Something went wrong..."
			<< std::endl;
		return false;
	}

	std::string	reader;
	std::getline(std::cin, reader);
	while (std::cin.good() && (reader.find(";;") == std::string::npos || reader.find(";;") == reader.find(';'))) {
		tmp_file.write(reader.c_str(), reader.size());
		tmp_file.write("\n", 1);
		std::getline(std::cin, reader);
	}
	if (std::cin.good()) {
		std::string tmp(reader.begin(), reader.begin() + reader.find(';'));
		tmp_file.write((tmp + "\n").c_str(), tmp.size() + 1);
	}
	std::cin.ignore();
	std::cin.clear();

	tmp_file.close();
	return true;
}



static bool			checkFileExtansion(const std::string & file_name)
{
	auto ext_pos = file_name.rfind(".avm");
	if (ext_pos == std::string::npos || ext_pos + std::string(".avm").size() != file_name.size())
		return false;

	return true;
}

static void			processAvm(const std::string & input_file_name)
{
	if (checkFileExtansion(input_file_name)) {
		std::cout << "Error: Wrong file name!\n"
			<< "\t--->" << input_file_name
			<< "File should have \"avm\" extansion!"
			<< std::endl;
		return;
	}

	std::string	basic_file_name(input_file_name.begin(),
								input_file_name.end() - std::string(".avm").size());



	std::ifstream	input_file(basic_file_name + ".avm",
								std::ios_base::in);
	if (!input_file.is_open()) {
		std::cout << "Error: Cannot open file\n"
			<< "\t--->" << basic_file_name + ".avm"
			<< std::endl;
		return;
	}

	std::string::size_type fpath_pos = basic_file_name.rfind('/');
	if (fpath_pos != std::string::npos)
		basic_file_name.erase(basic_file_name.begin(), basic_file_name.begin() + fpath_pos);

	std::ofstream	output_file(basic_file_name + ".avmout",
								std::ios_base::out | std::ios_base::app | std::ios_base::trunc);
	if (!output_file.is_open()) {
		std::cout << "Error: Cannot open file\n"
			<< "\t--->" << basic_file_name + ".avmout"
			<< std::endl;
		input_file.close();
		return;
	}

	std::ofstream	log_file(basic_file_name + ".avmlog",
								std::ios_base::out | std::ios_base::app | std::ios_base::trunc);
	if (!log_file.is_open()) {
		std::cout << "Error: Cannot open file\n"
			<< "\t--->" << basic_file_name + ".avm"
			<< std::endl;
		output_file.close();
		input_file.close();
		return;
	}
	

	Avm avm(input_file, output_file, log_file);
	avm.check();
	avm.parse();
	avm.result();


	log_file.close();
	output_file.close();
	input_file.close();
}



int					main(const int argc, const char ** argv)
{
	if (argc == 1) {
		const std::string	tmp_file_name(generateTmpFileName());
		if (makeTmpFile(tmp_file_name))
			processAvm(tmp_file_name);
	}
	else
		for (register int i = 1; i < argc; i++)
			processAvm(argv[i]);

	int i;
	std::cin >> i;
	return 0;
}