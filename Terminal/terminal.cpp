/*
	Group Number - 53
	Group Member 1 - Suryam Arnav Kalra (19CS30050)
	Group Member 2 - Kunal Singh (19CS30025)
	Assignment - 2 (OS LAB)
*/

#include <bits/stdc++.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>
#include <sys/stat.h>
#include <dirent.h>
#include <ctime>
#include <iomanip>
using namespace std;

static sigjmp_buf env;
static volatile sig_atomic_t jump_active = 0;

static sigjmp_buf env2;
static volatile sig_atomic_t jump_active2 = 0;


// Function to Re-store the command history from the hidden file
void get_commands(deque<string> &command_history){
	int file_d = open(".myShell_history.txt", O_RDONLY);
	if(file_d < 0){
		return;
	}
	string cmd = "";
	while(1){
		char read_file[10];
		int bytes_from_file = read(file_d, read_file, 10-1);
		read_file[bytes_from_file] = '\0';
		int i = 0;
		while(i < strlen(read_file) && read_file[i] != '\0'){
			while(i < strlen(read_file) && read_file[i] != '\n'){
				cmd += read_file[i++];
			}
			if(read_file[i] == '\n'){
				command_history.push_back(cmd);
				cmd = "";
				i++;
			}
		}
		if(bytes_from_file < 9){
			break;
		}
	}
	close(file_d);
	return;
}

// Singnal Handler for SIGINT signal
void sigint_handler(int signo) {
    if (!jump_active) {
        return;
    }
    siglongjmp(env, 42);
}

// Function used to execute the command along with all the arguments provided
void execute_command(string command, int input_d, int output_d) {
	vector<string> tokens;
	int n = command.size();
	int prev = 0;
	int i = 0;
	while(i < n) {
		while(i < n && command[i] == ' ') {
			i++;
		}
		prev = i;
		if(i < n && command[i] == '\'') {
			string temp = "";
			i++;
			while(i < n && command[i] != '\'') {
				temp += command[i];
				i++;
			}
			i++;
			tokens.push_back(temp);
			continue;
		}
		if(i < n && command[i] == '\"') {
			string temp = "";
			i++;
			while(i < n && command[i] != '\"') {
				temp += command[i];
				i++;
			}
			i++;
			tokens.push_back(temp);
			continue;
		}
		while(i < n && command[i] != ' ') {
			i++;
		}
		if(i > prev)
			tokens.push_back(command.substr(prev, i-prev));
	}

	bool input_redirection = 0, output_redirection = 0, background = 0;
	string input_file, output_file;
	int where = 0;
	for(int i = 0; i < (int)tokens.size(); i++) {
		if(tokens[i] == "<") {
			input_redirection = 1;
			input_file = tokens[i+1];
		}
		if(tokens[i] == ">") {
			output_redirection = 1;
			output_file = tokens[i+1];
		}
		if(tokens[i] == "&") {
			background = 1;
		}
		if(input_redirection == 0 && output_redirection == 0 && background == 0) {
			where++;
		}
	}

	const char **argv = new const char *[where+1];
    for (int i = 0; i < where; i++) {
        argv[i] = tokens[i].c_str();
    }
    argv[where] = NULL;

    pid_t pid, wpid;
    int status;

    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    pid = fork();
    if(pid == 0) {
    	if(input_redirection) {
    		input_d = open(input_file.c_str(), O_RDONLY);
    	}
    	if(output_redirection) {
    		output_d = open(output_file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
    	}
    	if(input_d != 0) {
    		dup2(input_d, 0);
    	}
    	if(output_d != 1) {
    		dup2(output_d, 1);
    	}
    	signal(SIGINT, SIG_DFL);
    	// signal(SIGTSTP, SIG_DFL);
    	execvp(tokens[0].c_str(), (char **)argv);
        exit(0);
    }
    else if(pid > 0) {
    	// signal(SIGINT, SIG_DFL);
    	signal(SIGTSTP, sigint_handler);
    	bool flag = 0;
    	if (sigsetjmp(env, 1) == 42) {
            cout << "Process running in background\n";
            flag = 1;
        }
        jump_active = 1;
    	if(flag) {
    		signal(SIGINT, SIG_DFL);
    		signal(SIGTSTP, SIG_DFL);
    		return;
    	}
    	if(!background) {
    		do {
                wpid = waitpid(pid,&status,WUNTRACED);
            }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    		if(input_d != 0) {
                close(input_d);
    		}
            if(output_d != 1) {
                close(output_d);
            }
    	}
    	else {
    		cout << "Process running in background\n";
    	}
    }
    else {
    	printf("Error in forking\n");
    	exit(1);
    }
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    return;
}

// The below blocks of code are of suffix automaton to find the longest common substring.
const int MAXLEN = 250052;
const int K = 256;

struct state {
	int length, link, next[K];
} st[MAXLEN*2-1];

int sz, last;
char A[MAXLEN], B[MAXLEN];

void init() {
	st[0].link = -1;
	memset(st[0].next, -1, sizeof st[0].next);
	sz++;
}

void extend(char c) {
	int nlast = sz++, p, q, clone;
	st[nlast].length = st[last].length + 1;
	memset(st[nlast].next, -1, sizeof st[nlast].next);
	for(p=last; p!=-1 && st[p].next[c]==-1; p=st[p].link) st[p].next[c] = nlast;
	if(p == -1) st[nlast].link = 0;
	else{
		q = st[p].next[c];
		if(st[p].length + 1 == st[q].length) st[nlast].link = q;
		else{
			clone = sz++;
			st[clone].length = st[p].length + 1;
			memcpy(st[clone].next, st[q].next, sizeof st[clone].next);
			st[clone].link = st[q].link;
			for(; p!=-1 && st[p].next[c]==q; p=st[p].link) st[p].next[c] = clone;
			st[q].link = st[nlast].link = clone;
		}
	}
	last = nlast;
}

int lcs(string a, string b) {
	int i, p, l, best, bestpos;
	init();
	for(i=0; a[i]; i++) extend ((int)a[i]);
	p = 0, l = 0, best = 0, bestpos = 0;
	for(i=0; b[i]; i++) {
		if(st[p].next[(int)b[i]] == -1) {
			for(; p!=-1 && st[p].next[(int)b[i]] == -1; p=st[p].link);
			if (p == -1) {
				p = l = 0;
				continue;
			}
			l = st[p].length;
		}
		p = st[p].next[(int)b[i]]; l++;
		if(l > best) best = l, bestpos = i;
	}
	return best;
}

int jump = 0;

// Signal Handler for SIGCHILD signal
void handle_sigchld(int sig) {
    jump = 1;
}

// Function to implement multiWatch
void multiwatch(string command) {
	signal(SIGINT, SIG_IGN);
	vector<string> cmds;
	int n = command.size();
	int i = 10;
	while(i < n && command[i] == ' ') {
		i++;
	}
	i++;
	while(i < n) {
		while(i < n && command[i] == ' ') {
			i++;
		}
		string cmd = "";
		i++;
		while(i < n && command[i] != ',' && command[i] != ']') {
			cmd += command[i];
			i++;
		}
		cmd.pop_back();
		
		cmds.push_back(cmd);
		if(command[i] == ']'){
			i++;
			break;
		}
		i++;
	}

	int file_d_multiwatch = 1;

	while(i < n){
		while(i < n && command[i] == ' '){
			i++;
		}
		if(command[i] == '>'){
			i++;
			while(i < n && command[i] == ' '){
				i++;
			}
			string file_name = "";
			while(i < n){
				file_name += command[i++];
			}	
			file_d_multiwatch = open(file_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);		
		}
	}


	int num_commands = cmds.size();
	vector<int> child_pid(num_commands);

	for(int i = 0; i< num_commands; i++) {
		pid_t pid = fork();
		if(pid == 0) {
			signal(SIGINT, SIG_DFL);
			string out_file = ".temp.";
			out_file += to_string((int)getpid());
			out_file += ".txt";
			int f = open(out_file.c_str(), O_RDWR | O_CREAT, 0644); // Creation of a seperate hidden file for each command
			command = cmds[i];
			vector<string> tokens;
			int n = command.size();
			int prev = 0;
			int i = 0;
			while(i < n) {
				while(i < n && command[i] == ' ') {
					i++;
				}
				prev = i;
				if(i < n && command[i] == '\'') {
					string temp = "";
					i++;
					while(i < n && command[i] != '\'') {
						temp += command[i];
						i++;
					}
					i++;
					tokens.push_back(temp);
					continue;
				}
				if(i < n && command[i] == '\"') {
					string temp = "";
					i++;
					while(i < n && command[i] != '\"') {
						temp += command[i];
						i++;
					}
					i++;
					tokens.push_back(temp);
					continue;
				}
				while(i < n && command[i] != ' ') {
					i++;
				}
				if(i > prev)
					tokens.push_back(command.substr(prev, i-prev));
			}

			bool input_redirection = 0, output_redirection = 0, background = 0;
			string input_file, output_file;
			int where = 0;
			// Handling of input output redirection and background process.
			for(int i = 0; i < (int)tokens.size(); i++) {
				if(tokens[i] == "<") {
					input_redirection = 1;
					input_file = tokens[i+1];
				}
				if(tokens[i] == ">") {
					output_redirection = 1;
					output_file = tokens[i+1];
				}
				if(tokens[i] == "&") {
					background = 1;
				}
				if(input_redirection == 0 && output_redirection == 0 && background == 0) {
					where++;
				}
			}
			int input_d = 0, output_d = f;
			output_redirection = 1;
			output_file = out_file;
			const char **argv = new const char *[where+1];
		    for (int i = 0; i < where; i++) {
		        argv[i] = tokens[i].c_str();
		    }
		    argv[where] = NULL;
		    pid = fork();
		    if(pid == 0) {
			    if(input_redirection) {
		    		input_d = open(input_file.c_str(), O_RDONLY);
		    	}
		    	if(output_redirection) {
		    		// output_d = open(output_file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
		    	}
		    	if(input_d != 0) {
		    		dup2(input_d, 0);
		    	}
		    	if(output_d != 1) {
		    		dup2(output_d, 1);
		    	}
		    	if(execvp(tokens[0].c_str(), (char **)argv) < 0){
		    		exit(1);
		    	}
		    	// execvp(tokens[0].c_str(), (char **)argv);
		        exit(0);
		    }
			else {
				fd_set fds;
				FD_ZERO(&fds);
				int prev = 0;
				while(1) {
					FD_ZERO(&fds);
					FD_SET(f, &fds);
					// struct sigaction sa;
					// sa.sa_handler = &handle_sigchld;
					int status;
					waitpid(pid,&status,WNOHANG | WUNTRACED);
					if(WIFEXITED(status)){
						int es = WEXITSTATUS(status);
						// cout<<"Exit status: "<<es<<"\n";
						if(es != 0){
							break;
						}	
					}
					sigset_t pselect_set;
					sigemptyset(&pselect_set);
   					sigaddset(&pselect_set, SIGCHLD);
   					signal(SIGCHLD, handle_sigchld);
   					jump_active2 = 1;
					struct timespec tv;
					tv.tv_sec = 5;
    				tv.tv_nsec = 0;
					int n_fd = pselect(f+1, &fds, NULL, NULL, NULL, &pselect_set);
					if(n_fd <= 0) {
						break;
					}
					if(FD_ISSET(f, &fds)) {
						int temp = f;
						int offset = 0;
						temp = f;
						int new_offset = lseek(temp, 0, SEEK_END);
						if(new_offset == prev) {
							continue;
						}
						string output = "";
						output += command;
						output += ",  current time: ";
						std::locale::global(std::locale("en_US.utf8")); // For printing the unix timestamp with each command
						time_t seconds = time(0);
						char buf[100];
					    strftime(buf, sizeof buf, "%c\n", std::localtime(&seconds));
						for(int j = 0; buf[j]; j++) {
							output += buf[j];
						}
						for(int j = 0; j< 20; j++) {
							output += "<-";
						}
						output += "\n";
						output += "\0";
						const char *w = output.c_str();
						write(file_d_multiwatch, w, strlen(w));
						output = "";
						temp = f;
						lseek(temp, 0, SEEK_SET);
						while(1) {
							bzero(buf, sizeof(buf));
							int bytes_read = read(temp, buf, sizeof(buf)-1);
							offset += bytes_read;
							if(bytes_read <= 0) {
								break;
							}
							write(file_d_multiwatch, buf, strlen(buf));
						}
						prev = offset;
						for(int j = 0; j < 20; j++) {
							output += "->";
						}
						output += "\n";
						w = output.c_str();
						write(file_d_multiwatch, w, strlen(w));
					}
					
					FD_CLR(f, &fds);
					if (jump == 1) {
			            break;
			        }
				}
				exit(0);
			}
		}
		else {
			child_pid[i] = (int)pid;
		}
	}
	
	int status, wpid;
	for(pid_t pid: child_pid) {
		do {
	        wpid = waitpid(pid,&status,WUNTRACED);
	    }while(!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	// Removing the Temporary hidden files created for each command
	for(int pid: child_pid) {
		string file = ".temp.";
		file += to_string(pid);
		file += ".txt";
		string command = "rm ";
		command += file;
		execute_command(command, 0, 1);
	}
	if(file_d_multiwatch != 1){
		close(file_d_multiwatch);
	}
	signal(SIGINT, SIG_DFL);
	return;
}

void print_spaces(int n){
	while(n--){
		cout<<" ";
	}
	return;
}

// Printing myShell> based on the color selected currently
void print_myShell(int foreground_color, int background_color){
	if(foreground_color == 0 && background_color == 0){
		cout<<"mySHELL> ";
	}
	else if(foreground_color == 0){
		cout<<"\033[1;37;";
		cout<<background_color<<"mmySHELL> \033[0m";
	}
	else if(background_color == 0){
		cout << "\033[1;"<<foreground_color<<"mmySHELL> \033[0m";
	}
	else{
		cout << "\033[1;"<<foreground_color<<";"<<background_color<<"mmySHELL> \033[0m";
	}
}

int main() {

	deque<string> command_history;

	get_commands(command_history);

	int foreground_color = 33, background_color = 0;


	while(1) {
		print_myShell(foreground_color, background_color);
		
		fflush(stdout);
		string command = "";

		auto global_it = (command_history.end());
		struct termios oldSettings, newSettings;

	    tcgetattr(fileno(stdin), &oldSettings);
	    newSettings = oldSettings;
	    newSettings.c_lflag &= (~ICANON & ~ECHO);
	    tcsetattr(fileno(stdin), TCSANOW, &newSettings);  

	    while(1) {
	        char c;
            read(fileno(stdin), &c, 1);
            if(c == '\n') {
            	char x = '\n';
            	write(1, &x,1);
            	break;
	        }

	        // up arrow and down arrow key
	        if(c == 27) {
	        	read(fileno(stdin), &c, 1);
	        	read(fileno(stdin), &c, 1);
	        	if(c == 'A') {
	        		while(!command.empty()) {
	        			const char delbuf[] = "\b \b";
			            write(STDOUT_FILENO, delbuf, strlen(delbuf));
		            	command.pop_back();
	        		}
	        		if(global_it != command_history.begin()) {
	        			global_it--;
	        		}
	        		command = *global_it;
	        		
	        		const char *temp = command.c_str();
	        		write(1, temp, strlen(temp));
	        	}
	        	else if(c == 'B') {
	        		while(!command.empty()) {
	        			const char delbuf[] = "\b \b";
			            write(STDOUT_FILENO, delbuf, strlen(delbuf));
		            	command.pop_back();
	        		}
	        		if(global_it != prev(command_history.end())) {
	        			global_it++;
	        		}
	        		command = *global_it;
	        		
	        		const char *temp = command.c_str();
	        		write(1, temp, strlen(temp));
	        	}
	        	continue;
	        }

	        // handle ctrl+r event
	        if(c == 18) {
	        	tcsetattr(fileno(stdin), TCSANOW, &oldSettings);
	        	cout << "\nEnter search prompt: ";
	        	string search;
	        	getline(cin, search);
	        	bool exact_match = false;
	        	if((int)command_history.size() == 0) {
	        		cout << "No match for search term in history" << endl;
	        		break;
	        	}
	        	auto it = prev(command_history.end());
	        	while(1) {
	        		if((*it) == search) {
	        			exact_match = true;
	        			cout << "Most recent exact match = " << *it << endl;
	        			break;
	        		}
	        		if(it == command_history.begin()) {
	        			break;
	        		}
	        		it--;
	        	}
	        	bool substring_match = false;
	        	if(!exact_match) {
	        		vector<pair<int, string>> v;
	        		for(string from: command_history) {
	        			int match_length = lcs(from, search);
	        			if(match_length > 2) {
	        				v.push_back({match_length, from});
	        			}
	        		}
	        		if((int)v.size() != 0) {
	        			sort(v.rbegin(), v.rend());
			        	int max_match = v[0].first;
			        	for(int i = 0; i < (int)v.size(); i++) {
			        		if(v[i].first == max_match) {
			        			substring_match = true;
			        			cout << v[i].second << endl;
			        		}
			        		else {
			        			break;
			        		}
			        	}
	        		}
	        	}
	        	
	        	if(!substring_match && !exact_match) {
	        		cout << "No match for search term in history\n";
	        	}
                break;
            }

	        // backspace key
	        if(c == 127) {
	        	if(command != "") {
		            const char delbuf[] = "\b \b";
		            write(STDOUT_FILENO, delbuf, strlen(delbuf));
	            	command.pop_back();
	            }
	            continue;
	        }

	        // handle tab key auto completion
	        if(c == '\t') {
	        	DIR *dir;
				struct dirent *ent;
				dir = opendir(".");
				vector<string> files;
				while ((ent = readdir (dir)) != NULL) {
					struct stat statbuf;
					if(lstat(ent->d_name, &statbuf) < 0) {
						printf("error\n");
					}
					if(S_ISDIR(statbuf.st_mode) == 0) {
						files.push_back(ent->d_name);
					}
				}
				string temp_file = "";
				for(int i = (int)command.size()-1; i >= 0; i--) {
					if(command[i] == ' ') {
						break;
					}
					temp_file += command[i];
				}
				reverse(temp_file.begin(), temp_file.end());
				int num_match = 0;
				vector<int> file_index;
				for(int i = 0; i< (int)files.size(); i++) {
					string file = files[i];
					int t_length = temp_file.size();
					if(file.substr(0, t_length) == temp_file) {
						num_match++;
						file_index.push_back(i);
					}
				}
				if((int)file_index.size() > 0) {
					if(num_match == 1) {
						string file =files[file_index[0]];
						int t_length = temp_file.size();
						string rem_name = file.substr(t_length);
						command += rem_name;
						const char *rem = rem_name.c_str();
						write(1, rem, strlen(rem));
					}
					else {
						string temp = "";
						temp += "\n";
						for(int i = 0; i < (int)file_index.size(); i++) {
							string file = files[file_index[i]];
							temp += to_string(i+1);
							temp += ". ";
							temp += file;
							temp += " ";
						}
						temp += "\n";
						const char *temp_buf = temp.c_str();
						write(1, temp_buf, strlen(temp_buf));
						temp = "\n";
						temp += "Enter the index: ";
						temp_buf = temp.c_str();
						write(1, temp_buf, strlen(temp_buf));
						string integer = "";
						while(1) {
							char x;
							read(0, &x, 1);
							if(x == '\n') {
								write(1, &x, 1);
								break;
							}
							if(x == 127) {
								if(integer != "") {
						            const char delbuf[] = "\b \b";
						            write(STDOUT_FILENO, delbuf, strlen(delbuf));
					            	integer.pop_back();
					            }
					            continue;
							}
							write(1, &x, 1);
							
							integer += x;
						}
						int index = stoi(integer);
						index--;
						if(index < 0 || index > file_index.size()-1){
							const char *rem = command.c_str();
							write(1, rem, strlen(rem));
						}
						else{
							string file =files[file_index[index]];
							int t_length = temp_file.size();
							string rem_name = file.substr(t_length);
							command += rem_name;
							const char *rem = command.c_str();
							write(1, rem, strlen(rem));
						}
					}
				}
				closedir (dir);
	            continue;
	        }
            write(1, &c, 1);
            command += c;
        }
        
	    tcsetattr(fileno(stdin), TCSANOW, &oldSettings);

		
		if(command == "") {
			continue;
		}
		

		if(command == "quit") {
			int file_d = open(".myShell_history.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

			for(string x : command_history){
				string temp = x + "\n";
				const char *temp_ = temp.c_str();
				write(file_d, temp_, strlen(temp_));
			}

			close(file_d);
			break;
		}

		if(command == "history") {
			auto it = command_history.end();
			int cnt = min(10000, (int)command_history.size());
			while(cnt--) {
				--it;
				cout << *it << endl;
			}
			command_history.push_back(command);
			if(command_history.size() > 10000) {
				command_history.pop_front();
			}
			continue;
		}

		command_history.push_back(command);
		// Check to keep only recent 10000 commands
		if(command_history.size() > 10000) {
			command_history.pop_front();
		}

		if((int)command.size() > 10 && command.substr(0, 10) == "multiWatch") {
			multiwatch(command);
			continue;
		}

		if(command == "changeColor"){
			cout<<"\n";
			map<string,pair<int,int>> m;
			m["black"] = {30,40};
			m["red"] = {31,41};
			m["green"] = {32,42};
			m["yellow "] = {33,43};
			m["blue "] = {34,44};
			m["magenta"] = {35,45};
			m["cyan "] = {36,46};
			m["white"] = {37,47};
			m["transparent "] = {0,0};

			cout<<"      Color     |Foreground color|Background color\n";
			cout<<"==================================================\n";

			for(auto x : m){
				int padlen1 = (16-x.first.size())/2;
				int padlen2 = (16-2)/2;
				int padlen3 = (16-2)/2;
				print_spaces(padlen1);
				cout<<x.first;
				print_spaces(padlen1);
				print_spaces(padlen2);
				cout<<x.second.first;
				print_spaces(padlen2);
				print_spaces(padlen3);
				cout<<x.second.second;
				print_spaces(padlen3);
				cout<<"\n";
			}
			cout<<"\n";
			cout<<"Enter the code for foreground color: ";
			cin >> foreground_color;
			cout<<"Enter the code for background color: ";
			cin >> background_color;
			continue;			
		}

		vector<string> cmds;
		int prev = 0;
		int n = command.size();
		for(int i = 0; i < n; i++) {
			if(command[i] == '|') {
				cmds.push_back(command.substr(prev, i-prev));
				prev = i+1;
			}
		}
		if(n != 0)
			cmds.push_back(command.substr(prev));

		int total_commands = (int)cmds.size();

		if(total_commands == 0) {
			continue;
		}

		int num_pipes = total_commands-1;

		int **pipes = new int*[num_pipes];

		for(int i = 0; i < num_pipes; i++) {
			pipes[i] = new int[2];
			pipe(pipes[i]);
		}

		if(total_commands == 1) {
			int input_d = 0, output_d = 1;
			execute_command(cmds[0], input_d, output_d);
		}
		else {
			for(int i = 0; i< total_commands; i++) {
				int input_d = 0, output_d = 1;
				if(i > 0) {
					input_d = pipes[i-1][0];
				}
				if(i < total_commands-1) {
					output_d = pipes[i][1];
				}
				execute_command(cmds[i], input_d, output_d);
			}
		}
		fflush(stdin);
		fflush(stdout);
	}
	return 0;
}