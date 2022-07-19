#include <iostream>
#include <sstream>
#include <string>
#include <array>
#include <direct.h>

using namespace std;

void print_help()
{
	cout << "Video Splitter v1.0" << endl << endl;
	cout << "Usage:" << endl;
	cout << "video-splitter [options] video-file.mp4" << endl << endl;
	cout << "Options:" << endl;
	cout << "-c    count    video count to generate (ignores -s parameter)" << endl;
	cout << "-s    seconds  time for every video (default 30)" << endl;
}

string cmd_exec(const char* cmd)
{
	array<char, 128> buffer;
	string result;
	unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe)
	{
		cerr << "_popen() failed!" << endl;
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}
	return result;
}

string get_file_extension(const char* file_name)
{
	string s = string(file_name);
	size_t pos = s.rfind('.');
	if (pos != string::npos)
	{
		return s.substr(pos + 1, s.length() - 1);
	}
	return "";
}

string get_file_name_without_extension(const char* file_name)
{
	string s = string(file_name);
	size_t path_pos = s.rfind('\\');
	size_t path_pos_2 = s.rfind('/');
	if (path_pos_2 != string::npos && path_pos_2 > path_pos)
	{
		path_pos = path_pos_2;
	}
	if (path_pos != string::npos)
	{
		s = s.substr(path_pos + 1, s.length() - 1);
	}
	size_t dot_pos = s.rfind('.');
	if (dot_pos != string::npos)
	{
		return s.substr(0, dot_pos);
	}
	return file_name;
}

string get_parent_path(const char* file_name)
{
	string s = string(file_name);
	size_t path_pos = s.rfind('\\');
	size_t path_pos_2 = s.rfind('/');
	if (path_pos_2 != string::npos && path_pos_2 > path_pos)
	{
		path_pos = path_pos_2;
	}
	if (path_pos != string::npos)
	{
		return s.substr(0, path_pos + 1); // include path seperator at end
	}
	return "";
}

int main(int argc, char** argv) {

	if (argc == 1)
	{
		print_help();
		return 0;
	}

	int count = -1;
	int part_seconds = 30;
	char* file_name = nullptr;

	if (argc == 2)
	{
		file_name = argv[1];
		cout << "File name is " << file_name << endl;
	}
	else if (argc == 4)
	{
		file_name = argv[3];
		cout << "File name is " << file_name << endl;

		char* option = argv[1];

		if (strcmp("-c", option) == 0)
		{
			count = atoi(argv[2]);
			cout << "-c parameter specified as " << count << endl;
		}
		else if (strcmp("-s", option) == 0)
		{
			part_seconds = atoi(argv[2]);
			cout << "-s parameter specified as " << part_seconds << endl;
		}
		else
		{
			cerr << "Invalid options specified!" << endl;
			return -1;
		}
	}
	else
	{
		cerr << "Invalid parameters!" << endl;
		
		for (int i = 0; i < argc; i++) {
			cout << "#" << i << ": " << argv[i] << endl;
		}
		
		return -1;
	}

	string cmd = "ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 -i ";
	cmd += file_name;

	string video_duration = cmd_exec(cmd.c_str());

	int video_seconds = atoi(video_duration.c_str());

	cout << "Video duration is " << video_seconds << " seconds" << endl;

	if (count != -1)
	{
		part_seconds = video_seconds / count;
		cout << "Count parameter using (" << count << "), every part will be " << part_seconds << " seconds" << endl;
	}

	int part_count = video_seconds / part_seconds + (video_seconds % part_seconds == 0 ? 0 : 1);

	cout << part_count << " parts will be generated - durations limited to " << part_seconds << "s" << endl << endl;

	string base_name = get_file_name_without_extension(file_name);
	string ext = get_file_extension(file_name);
	string parent_path = get_parent_path(file_name);

	cout << "Detected file base name: " << base_name << endl;
	cout << "Detected file extension: " << ext << endl << endl;

	for (int i = 0; i < part_count; i++)
	{
		int ss = (i * part_seconds);
		int t = part_seconds;

		cout << "# " << (i + 1) << "/" << part_count << " (" << ss << " - ";
		cout << ss + (i != part_count -1 ? t : video_seconds - i * part_seconds) << ")" << endl;

		ostringstream out_file;
		out_file << parent_path << base_name << "_part_" << (i < 9 ? "0" : "") << (i + 1) << "." << ext;

		string out_file_name = out_file.str();

		cout << out_file_name << endl << endl;

		ostringstream cmd;
		cmd << "ffmpeg -v error -ss " << ss << " -t " << t << " -i " << file_name << " ";
		cmd << out_file_name;

		// comment below to bypass execution of ffmpeg!!
		cmd_exec(cmd.str().c_str());
	}

	return 0;
}
