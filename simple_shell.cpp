#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#include <string.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

bool get_input(std::string &input, bool prompt) {
	if(!prompt) {
		cout << "shell: ";
	}
	getline(cin, input);
	return !cin.eof();
}

//function for checking invalid inputs
string check_for_invalid_input(string input) {

	//check for multiple ampersand and ampersand in middle of file
	vector<string> parse1;
	int amp_found = 0;
	string delim = " ";
	string token;
	string s = input;
	while(s.find(delim) != string::npos) {
		token = s.substr(0, s.find(delim));
		if(token.length() > 0) {
			parse1.push_back(token);
		}
		s.erase(0, s.find(delim) + delim.length());
	}
	token = s.substr(0, s.find(delim));
	if(token.length() > 0) {
		parse1.push_back(token);
	}

	bool ampInBack = false;
	if(parse1.size() > 0) {
		if(static_cast<int>(parse1[parse1.size()-1][parse1[parse1.size()-1].length()-1]) == 38) {
			ampInBack = true;
		}
	}

	for(int i = 0; i < parse1.size(); i++) {
		for(int j = 0; j < parse1[i].length(); j++) {
			if(static_cast<int>(parse1[i][j]) == 38 && (i != parse1.size()-1 ||  i == parse1.size()-1 && j != parse1[i].length()-1)) {
				if(ampInBack) {
					return "Multiple & detected";
				} else {
					return "Found & in the middle of input";
				}
			}
		}
	}

	//Create a vector of our input parsed with "|"
	s = input;
	delim = "|";
	vector<string> piped_input;
	while(s.find(delim) != string::npos) {
		token = s.substr(0, s.find(delim));
		if(token.length() > 0) {
			piped_input.push_back(token);
		}
		s.erase(0, s.find(delim) + delim.length());
	}
	token = s.substr(0, s.find(delim));
	if(token.length() > 0) {
		piped_input.push_back(token);
	}

	vector<vector<string> > piped_input_2;
	delim = " ";
	for(int i = 0; i < piped_input.size(); i++) {
		s = piped_input[i];
		vector<string> temp;

		while(s.find(delim) != string::npos) {
			token = s.substr(0, s.find(delim));
			if(token.length() != 0 && token.compare("&") != 0) {
				temp.push_back(token);
			}
			s.erase(0, s.find(delim) + delim.length());
		}
		token = s.substr(0, s.find(delim));
		if(token.length() != 0 && token.compare("&") != 0) {
			temp.push_back(token);
		}

		piped_input_2.push_back(temp);
	}

	for(int i = 0; i < piped_input_2.size(); i++) {
		for(int j = 0; j < piped_input_2[i].size(); j++) {
			if(j == 0 && static_cast<int>(piped_input_2[i][0][0]) == 60) {
				return "Detected < at the beginning of a command";
			}
			if(j == 0 && static_cast<int>(piped_input_2[i][0][0]) == 62) {
				return "Detected > at the beginning of a command";
			}

			if(j == piped_input_2[i].size() - 1 && piped_input_2[i][piped_input_2[i].size()-1][piped_input_2[i][piped_input_2[i].size()-1].length()-1] == 60) {
				return "Detected < at the end of a command";
			}
			if(j == piped_input_2[i].size() - 1 && piped_input_2[i][piped_input_2[i].size()-1][piped_input_2[i][piped_input_2[i].size()-1].length()-1] == 62) {
				return "Detected > at the end of a command";
			}
			
		}
	}

	//detecting | at beginning and end of input
	vector<string> parse2;
	delim = " ";
	s = input;
	while(s.find(delim) != string::npos) {
		token = s.substr(0, s.find(delim));
		if(token.length() > 0) {
			parse2.push_back(token);
		}
		s.erase(0, s.find(delim) + delim.length());
	}
	token = s.substr(0, s.find(delim));
	if(token.length() > 0) {
		parse2.push_back(token);
	}
	if(static_cast<int>(parse2[0][0]) == 124 || static_cast<int>(parse2[parse2.size()-1][parse2[parse2.size()-1].size()-1]) == 124 ) {
		return "Detected | at the beginning and end of input";
	}


	//check for duplicate meta character consecutively
	int prev_pipe = 0;
	int prev_lt = 0;
	int prev_gt = 0;

	for(int i = 1; i < input.length(); i++) {
		if(static_cast<int>(input[i]) == 124) {
			if(prev_pipe == 1) {
				return "Cannot have two | consecutively";
			}
			prev_pipe = 1;
		} else {
			prev_pipe = 0;
		}

		if(static_cast<int>(input[i]) == 60) {
			if(prev_lt == 1) {
				return "Cannot have two < consecutively";
			}
			prev_lt = 1;
		} else {
			prev_lt = 0;
		}

		if(static_cast<int>(input[i]) == 62) {
			if(prev_gt == 1) {
				return "Cannot have two > consecutively";
			}
			prev_gt = 1;
		} else {
			prev_gt = 0;
		}
	}

	//dealing with < and >
	vector<string> parse3;
	delim = "|";
	s = input;
	while(s.find(delim) != string::npos) {
		token = s.substr(0, s.find(delim));
		if(token.length() > 0) {
			parse3.push_back(token);
		}
		s.erase(0, s.find(delim) + delim.length());
	}
	token = s.substr(0, s.find(delim));
	if(token.length() > 0) {
		parse3.push_back(token);
	}

	int lt_counter;
	int gt_counter;
	for(int i = 0; i < parse3.size(); i++) {
		lt_counter = 0;
		gt_counter = 0;
		if(i == 0) {
			for(int j = 0; j < parse3[i].length(); j++ ) {
				if(static_cast<int>(parse3[i][j]) == 60) {
					lt_counter++;
					if(lt_counter > 1) {
						return "Detected multiple < in the first command";
					}
				}

				if(static_cast<int>(parse3[i][j]) == 62) {
					if(parse3.size() > 1) {
						return "Detected > in first command. Conflicts with pipe input";
					} else {
						gt_counter++;
						if(gt_counter > 1) {
							return "Detected multiple > in the first command";
						}
					}
				}
			}
		} else if (i == parse3.size()-1) {
			for(int j = 0; j < parse3[i].length(); j++ ) {
				if(static_cast<int>(parse3[i][j]) == 62) {
					gt_counter++;
					if(gt_counter > 1) {
						return "Detected multiple > in the first command";
					}
				}

				if(static_cast<int>(parse3[i][j]) == 60) {
					if(parse3.size() > 1) {
						return "Detected < in last command. Conflicts with pipes output";
					} else {
						lt_counter++;
						if(lt_counter > 1) {
							return "Detected Multiple < in the last command";
						}
					}
				}
			}

		} else {
			for(int j = 0; j < parse3[i].length(); j++) {
				if(static_cast<int>(parse3[i][j]) == 60) {
					return "Detected < in a middle command";
				}
				if(static_cast<int>(parse3[i][j]) == 62) {
					return "Detected > in a middle command";
				}
			}
		}
	}

	return "";
}

vector<vector<vector<string> > > parser(string input) {
	string str = input;

	int counter = 0;
	for(int i = str.length()-1; i>=0; i--) {
		if(static_cast<int>(str[i]) == 32) {
			counter++;
		} else if (static_cast<int>(str[i]) == 38) {
			str.erase(str.length()-1-counter, counter+1);
		} else {
			break;
		}
	}


	//string s = "cat -b -e tst.txt | ls | cat >tst.txt two three< tst2.txt>tst3.txt<tst4.txt>tst5.txt apple<tst6.txt<tst7.txt hundred | cat tst2.txt";
	string s = str;
	string delim = "|";
	string token;
	vector<vector<vector<string> > > parsed_input;

	//Create a vector of our input parsed with "|"
	vector<string> piped_input;
	while(s.find(delim) != string::npos) {
		token = s.substr(0, s.find(delim));
		piped_input.push_back(token);
		s.erase(0, s.find(delim) + delim.length());
	}
	piped_input.push_back(s.substr(0, s.find(delim)));				//Need to push the last input in. If it was blank, we need to know that too as that is invalid


	vector<vector<string> > piped_input_2;
	delim = " ";
	for(int i = 0; i < piped_input.size(); i++) {
		s = piped_input[i];
		vector<string> temp;

		while(s.find(delim) != string::npos) {
			token = s.substr(0, s.find(delim));
			if(token.length() != 0 && token.compare("&") != 0) {
				temp.push_back(token);
			}
			s.erase(0, s.find(delim) + delim.length());
		}
		token = s.substr(0, s.find(delim));
		if(token.length() != 0 && token.compare("&") != 0) {
			temp.push_back(token);
		}

		piped_input_2.push_back(temp);
	}
	
	/*
	int pipe_size = piped_input_2.size();
	if(pipe_size > 0) {
		int cmd_size = piped_input_2[pipe_size-1].size();
		if(cmd_size > 0) {
			int str_len = piped_input_2[pipe_size-1][cmd_size-1].length();
			if(str_len > 0) {
				if(static_cast<int>(piped_input_2[pipe_size-1][cmd_size-1][str_len-1]) == 38) {
					piped_input_2[pipe_size-1][cmd_size-1].erase(piped_input_2[pipe_size-1][cmd_size-1], 1);
				}
			}
		}
	}
	*/

	vector<vector<string> > piped_input_3;
	for(int i = 0; i < piped_input_2.size(); i++) {
		vector<string> temp;

		for(int j = 0; j < piped_input_2[i].size(); j++) {
			s = piped_input_2[i][j];
			delim = "<";

			if(s.find(delim) != string::npos) {
				while(s.find(delim) != string::npos) {

					token = s.substr(0, s.find(delim));
					if(token.length() != 0) {
						temp.push_back(token);
					}
					temp.push_back(delim);
					s.erase(0, s.find(delim)+delim.length());
				}
				token = s.substr(0, s.find(delim));
				if(token.length() != 0) {
					temp.push_back(token);
				}

			} else {
				temp.push_back(s);
			}
		}

		piped_input_3.push_back(temp);
	}

	vector<vector<string> > piped_input_4;
	for(int i = 0; i < piped_input_3.size(); i++) {
		vector<string> temp;

		for(int j = 0; j < piped_input_3[i].size(); j++) {
			s = piped_input_3[i][j];
			delim = ">";

			if(s.find(delim) != string::npos) {
				while(s.find(delim) != string::npos) {

					token = s.substr(0, s.find(delim));
					if(token.length() != 0) {
						temp.push_back(token);
					}
					temp.push_back(delim);
					s.erase(0, s.find(delim)+delim.length());
				}
				token = s.substr(0, s.find(delim));
				if(token.length() != 0) {
					temp.push_back(token);
				}

			} else {
				temp.push_back(s);
			}
		}

		piped_input_4.push_back(temp);
	}

	//parsed_input;
	for(int i = 0; i < piped_input_4.size(); i++) {
		bool lt_first = true;
		bool gt_first = true;
		bool lt_add = false;
		bool gt_add = false;
		bool check_next = false;
		vector<vector<string> > temp(3);
		vector<string> temp2;

		vector<string> empty;
		empty.push_back("");
		temp[1] = empty;
		temp[2] = empty;


		for(int j = 0; j < piped_input_4[i].size(); j++) {
			s = piped_input_4[i][j];
			if(lt_add) {
				vector<string> temp3;
				temp3.push_back(s);
				temp[1] = temp3;
				lt_add = false;
			} else if (gt_add) {
				vector<string> temp3;
				temp3.push_back(s);
				temp[2] = temp3;
				gt_add = false;
			} else if(s.compare("<") == 0) {
				lt_add = true;
			} else if (s.compare(">") == 0) {
				gt_add = true;
			} else {
				temp2.push_back(s);
			}
		}
		temp[0] = temp2;
		parsed_input.push_back(temp);
	}

	return parsed_input;
}

char* con_to_c_str(string str) {
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	return cstr;
}


void forkAndExec(char** args, string stdin, string stdout, int cmd_num, int total_cmd, int &status, int &p_fd_read) {
	
	pid_t child_pid, wpid;
	//int status = 0;

	// int p_fd[2];
	int fd[2];

	if(total_cmd > 1 && cmd_num != total_cmd - 1) {
		//Create a pipe except when it is last fork
		pipe(fd);
	}

	child_pid = fork();
	if(child_pid == 0) {

		if(cmd_num == 0) {

			//check if there is "<" by checking string length

			int in;
			int out;

			if(stdin.length() > 0) {
				in = open(stdin.c_str(), O_RDONLY, 0600);
			}

			if(stdout.length() > 0) {
				out = open(stdout.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
			}

			if(stdin.length() > 0) {
				close(0);
				dup2(in, 0);
				close(in);
			}

			if(total_cmd > 1) {
				close(1);
				close(fd[0]);

				dup2(fd[1], 1);
				close(fd[1]);
			} else if(stdout.length() > 0) {
				close(1);
				dup2(out, 1);
				close(out);
			}

		} else if (cmd_num == total_cmd - 1) {
			//THis is if it was the last command
			//Will only be used if there are 2 commands and this is the last one
			//Therefore, there will be a pipe before it and it will read from that pipe end

			int out;
			if(stdout.length() > 0) {
				out = open(stdout.c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0600);
			}

			
			close(0);
			dup2(p_fd_read, 0);
			close(p_fd_read);
			

			if(stdout.length() > 0) {
				close(1);
				dup2(out, 1);
				close(out);
			}

		} else {
			//Should be everything in the middle
			close(0);
			dup2(p_fd_read, 0);
			close(p_fd_read);
			
			close(1);
			close(fd[0]);
			dup2(fd[1], 1); 
			close(fd[1]);
		}


		//exec command
		if(execvp(args[0], args) == -1) {
			perror("ERROR: ");
		}


	} else {
		//parent
		if(total_cmd > 1) {

			if(cmd_num == 0) {
				p_fd_read = fd[0];
				close(fd[1]);
			} else if (cmd_num == total_cmd - 1) {
				close(p_fd_read);
			} else {
				close(fd[1]);
				close(p_fd_read);
				p_fd_read = fd[0];
			}


		}



		//while((wpid = wait(&status)) > 0);
	}

}

void deleteArray(char** arr, int n) {
	for(int i = 0; i < n; i++) {
		delete arr[i];
	}
	delete arr; //may not be optional? or might just break the whole thing idk
}

bool hasAmpersand(string input) {
	vector<string> parse1;
	int amp_found = 0;
	string delim = " ";
	string token;
	string s = input;

	while(s.find(delim) != string::npos) {
		token = s.substr(0, s.find(delim));
		if(token.length() != 0) {
			parse1.push_back(token);
		}
		s.erase(0, s.find(delim) + delim.length());
	}
	token = s.substr(0, s.find(delim));
	if(token.length() != 0) {
		parse1.push_back(token);
	}

	bool ampInBack = false;
	if(parse1.size() > 0) {
		if(static_cast<int>(parse1[parse1.size()-1][parse1[parse1.size()-1].length()-1]) == 38) {
			ampInBack = true;
		}
	}

	return ampInBack;
}

void signalHandler( int signum ) {

   wait(NULL);
   //exit(signum);  
}


int main(int argc, char *argv[]) {

	bool noPrint = false;

	//TODO: instead of checking argv[1], check all args for -n
	if(argc >= 2) {
		string arg1 = argv[1];
		if(arg1.compare("-n") == 0) {
			noPrint = true;
		}
	}

	string in;

	//Check for invalid inputs 
	while(get_input(in, noPrint)) {
		int status = 0;
		waitpid(-1, &status, WNOHANG);

		string inv_error = check_for_invalid_input(in);
		if(inv_error.length() > 0) {
			cout << "INVALID INPUT: " << inv_error << endl;
		} else {

			bool runInBackground = hasAmpersand(in);
			pid_t cid = 2;

			cid = fork();
			

			/*
			if(runInBackground && cid > 0) {
				signal(SIGCHLD, signalHandler);
			}
			*/


			if(cid==0) {
				//string input = "cat -b -e tst.txt | ls | cat >tst.txt two three< tst2.txt>tst3.txt<tst4.txt>tst5.txt apple<tst6.txt<tst7.txt hundred | cat tst2.txt";
				//string input = "ls";
				vector<vector<vector<string> > > parsed_input;
				parsed_input = parser(in);

				int p_fd_read;

				for(int i = 0; i < parsed_input.size(); i++) {
					//Fork and Pipe processes

					//This creates args from appropriate command
					vector<string> command = parsed_input[i][0];
					char* args[command.size() + 1];						//TODO::Delete memory in here
					for(int j = 0; j < command.size(); j++) {
						args[j] = con_to_c_str(command[j]);
					}
					args[command.size()] = NULL;

					forkAndExec(args, parsed_input[i][1][0], parsed_input[i][2][0], i, parsed_input.size(), status, p_fd_read);	
					//forkAndExec(args, "tst.txt", "tst2.txt", i, parsed_input.size());

					//delete args[command.size()+1] thing
					for(int j = 0; j < command.size(); j++) {
						delete(args[j]);
					}
				}

				for(int i = 0; i < parsed_input.size(); i++) {
					waitpid(-1, &status, 0);
				}

				if(cid == 0) {
					exit(0);

				}
			} else {
				if(!runInBackground) {
					waitpid(-1, &status, 0);
				}
					waitpid(-1, &status, WNOHANG);
			}
		}
		
	}
	return 0;






}