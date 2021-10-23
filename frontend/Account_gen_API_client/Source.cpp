#pragma once
#include <iostream>
#include <Windows.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <ostream>
using namespace std;
using nlohmann::json;
int main();
int check_whitelist();

// Gotta put Timeout in
//Global vars
struct stat s;
int choice;
int status;
string atype;
bool logged_out;
string accounts_;

// build in too many requests

class main_ {
public:
	string auth = "auth.json";
	cpr::Session req;
	string website = "localhost:5000";
	DWORD exit_code = 1;


	int check_whitelist_() {
		cpr::Session req;
		ifstream i;
		i.open("auth.json", ifstream::ate | ifstream::binary);
		int size_ = i.tellg();
		i.close();
		if (size_ != 0 && size_ != -1) {
			json j = json::parse(ifstream{ "auth.json", ios::in });
			req.SetHeader(cpr::Header{ { "auth", j["auth"]} });
			req.SetUrl(website + "/auth");
			req.SetTimeout(5000);
			cpr::Response check_auth = req.Get();
			if (check_auth.elapsed > 5000) {
				return 3;
			}
			if (check_auth.status_code == 200) {
				json j_ = json::parse(check_auth.text);
				if (j_["is_admin"] == true) {
					return 2;
				}
				else {
					return 1;
				}
			}
			else {
				cout << "Authentification check/Connection to the server failed. Exitting in 5 seconds." << endl;
				Sleep(5000);
			}
		}
		else {
			return 0;
		}
	}

	string Generate(string a, bool bypass_check) {
		switch(bypass_check) {
		case false: {
			json j = json::parse(ifstream{ "auth.json", ios::in });
			req.SetHeader(cpr::Header{ {"auth", j["auth"]}, {"account_type" , a} });
				req.SetUrl(website + "/get/account");
				req.SetTimeout(5000);
				cpr::Response account_gen = req.Get();
				if (account_gen.elapsed > 5000) {
					return "timeout";
				}
				if (account_gen.status_code == 200) {
					json j_ = json::parse(account_gen.text);
					if (j_["status"] == 400) {
						return "error";
					}
					else {
						return j_["account"];
					}
				}
				else {
					return "error";
				}
			}
		case true: {
			string a_ = a;
			check_whitelist();
			switch(status) {
			case 1: {
				Generate(a_, false);
			}
			case 2: {
				Generate(a_, false);
			}
			default: {
				cout << "Wrong authentification. Exitting in 5 seconds..." << endl;
				Sleep(5000);
				exit(1);
			}
			}
		}
		}
		}


	string Account_Types() {
		check_whitelist();
		cout << status;
		json j = json::parse(ifstream{ "auth.json", ios::in });
		if ((status == 1 || status == 2)) {
			req.SetHeader(cpr::Header{ {"auth", j["auth"]} });
			req.SetUrl(website + "/accounts");
			req.SetTimeout(5000);
			cpr::Response accounts = req.Get();
			if (accounts.elapsed > 5000) {
				return "timeout";
			}
			if (accounts.status_code == 200) {
				json j_ = json::parse(accounts.text);
				accounts_ = j_["account_type"];
			}
			else {
				return "error";
			}
		}
		else {
			return "error";
		}
	}

	int menu_logged_in() {
		cin >> choice;
		return choice;
	}

	int generate_prompt() {
		cout << "Type : " << endl;
		cin >> atype;
		string r = Generate(atype, false);
		if (r != "error") {
			cout << "Account Generated : " << r << " Press enter to return to menu." << endl;
			cin;
			main();
		}
		else {
			cout << "Some error occured. Please contact the owner." << endl;
			Sleep(5000);
			system("cls");
			main();
		}
		return 0;
	};
	int Account_prompt() {
		string types = Account_Types();
		if ((types != "error" || types != "timeout")) {
			cout << accounts_ << endl;
			Sleep(5);
			main();
		}
		else {
			cout << "Something went wrong. Please try again. " << endl;
			cin;
			system("cls");
			main();
		}
		return 0;
	}

	int Logout_prompt() {
		ofstream f;
		_unlink("auth.json");
		CreateFile(L"auth.json", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 2, FILE_ATTRIBUTE_NORMAL, NULL);
		f.open("auth.json", ios::in);
		f << "";
		f.close();
		cout << "Successfully logged out! Exitting.." << endl;
		Sleep(3);
		logged_out = true;
		return 0;
	}

	int Credit_prompt() {
		cout << "ScriptedWorld - Main developer" << endl;
		cin;
		main();
		return 0;
	}

	int login(string key) {
			req.SetUrl(website + "/auth");
			req.SetHeader(cpr::Header{ {"auth" , key} });
			req.SetTimeout(5000);
			cpr::Response login_r = req.Get();
			if(login_r.elapsed > 5000){
				return 3;
			}
			json j = json::parse(login_r.text);
			switch (login_r.status_code) {
			case 200: {
				if (j["is_admin"] == true) {
					return 2;
				}
				else {
					return 1;
				}
			case 403: {
				return 0;
			}
			case 429: {
				return 0;
			}
			}
			}
		return 50;
	}
	int timeout_elapsed() {
		system("cls");
		main();
		return 0;
	}
};

class admin : public main_ {
public:
	string create_whitelist(string plan) {
		json j = json::parse(ifstream{ "auth.json", ios::in });
		req.SetUrl(website + "/admin/create/whitelist");
		req.SetHeader(cpr::Header{ {"auth", j["auth"]}, {"plan", plan} });
		req.SetTimeout(5000);
		cpr::Response c_whitelist = req.Get();
		if (c_whitelist.elapsed > 5000) {
			return "timeout";
		}
		cout << c_whitelist.text << endl;
		json j_ = json::parse(c_whitelist.text);
		if (c_whitelist.status_code == 200) {
			if (j_["status"] != 405) {
				return j_["whitelist"];
			}
			else {
				return "error";
			}
		}
	}

		int add_accounttype(string type) {
			json j = json::parse(ifstream{ "auth.json", ios::in });
			req.SetUrl(website + "/admin/add_account_type");
			req.SetHeader(cpr::Header{ {"auth", j["auth"]}, {"name" , type} });
			req.SetTimeout(5000);
			cpr::Response create_acctype = req.Get();
			if (create_acctype.elapsed > 5000) {
				return 3;
			}
			if (create_acctype.status_code == 200) {
				return 0;
			}
			else {
				return 1;
			}
		}
	

		int remove_accounttype(string type) {
			json j = json::parse(ifstream{ "auth.json", ios::in });
			req.SetUrl(website + "/admin/remove/plan");
			req.SetTimeout(5000);
			req.SetHeader(cpr::Header{ {"name", type}, { "auth" , auth } });
			cpr::Response remove_acctype = req.Get();
			if (remove_acctype.elapsed < 5000) {
				return 3;
			}
			if (remove_acctype.status_code == 200) {
				return 0;
			}
			else if (remove_acctype.status_code == 404) {
				return 1;
			}
			else if (remove_acctype.status_code == 429) {
				return 1;
			}
			else {
				return 2;
			}
		}

		int lock_accounttype(string type) {
			json j = json::parse(ifstream{ "auth.json", ios::in });
			req.SetUrl(website + "/admin/lock_account_type");
			req.SetHeader(cpr::Header{ {"auth", j["auth"]}, {"plan" , type} });
			req.SetTimeout(5000);
			cpr::Response lock_acctype = req.Post();
			if (lock_acctype.elapsed < 5000) {
				return 3;
			}
			if (lock_acctype.status_code == 200) {
				cout << lock_acctype.text << endl;
				json j_ = json::parse(lock_acctype.text);
				if (j_["status"] == 200) {
					return 0;
				}
			}
			else if (lock_acctype.status_code == 400) {
				return 1;
			}
			else {
				return 2;
			}
		}
	};

int check_whitelist() {
	main_ m;
	status = m.check_whitelist_();
	return 0;
}

int main(){
	if (logged_out == true) {
		exit(1);
	}
	main_ m;
	check_whitelist();
	switch (status) { // Can also see exit code via Thread (easy solution)
	case 1:
		cout << "Welcome back!\n\n1 - Generate\n2 - Accounts\n3 - Logout\n4 - Credits" << endl;
		m.menu_logged_in();
		switch (choice) {
		case 1: {
			m.generate_prompt();
		}
		case 2: {
			m.Account_prompt();
		}
		case 3: {
			m.Logout_prompt();
		}
		case 4: {
			m.Credit_prompt();
		}

		default:
			cout << "Invalid Syntax! Please try something else. " << endl;
			system("cls");
			main();
		}
	case 2: {
		cout << "Welcome, admin!\n1 - Generate\n2 - Accounts\n3 - Blacklist\n4 - Whitelist\n5 - Add account type\n6 - Remove account type\n7 - Lock account type\n8 - Unlock account type\n9 - Logout\n10 - Credits" << endl;
		m.menu_logged_in();
		switch (choice) {
		case 1: {
			m.generate_prompt();
		}
		case 2: {
			m.Account_prompt();
		}
		case 3: {
			m.Logout_prompt();
			exit(1);
		}
		case 4: {
			admin a;
			string plan;
			cout << "Plan? : " << endl;
			cin >> plan;
			string result = a.create_whitelist(plan);
			if (result != "error") {
				cout << "Success! Whitelist key : " << result << endl;
				cin;
				main();
			}
			else {
				cout << "Something went wrong. Please try again. " << endl;
				cin;
				main();
			}
		}
		case 5: {
			admin a;
			string p;
			cout << "Plan to create? : " << endl;
			cin >> p;
			int result = a.add_accounttype(p);
			switch (result) {
			case 0: {
				cout << "Success!" << endl;
				Sleep(3);
				main();
			}
			case 1: {
				cout << "Invalid auth. " << endl;
				Sleep(3);
				exit(0);
			}
			}
		}
		case 6: {
			admin a;
			string p;
			cout << "Plan to remove? : " << endl;
			cin >> p;
			int result = a.remove_accounttype(p);
			switch (result) {
			case 1:
				cout << "Success! " << endl;
				cin;
			case 2: {
				cout << "Invalid auth. " << endl;
				cin;
				exit(0);
			}
			case 3: {
				m.timeout_elapsed();
			}
			}
		}

		case 7: {
			admin a;
			string p;
			cout << "Account type to lock? : " << endl;
			cin >> p;
			int result = a.lock_accounttype(p);
			switch (result) {
			case 0: {
				cout << "Success!" << endl;
				cin;
				main();
			}
			case 1: {
				cout << "Account type already frozen. " << endl;
				cin;
				main();
			}
			case 2: {
				cout << "Invalid auth!" << endl;
				cin;
				exit(0);
			}
			case 3: {
				m.timeout_elapsed();
			}
			}
		}
		case 8: {
			exit(0);
		}
		}
	};
	case 0: {
		cout << "Welcome! It seems like you are new.\n1 - Login\n2 - Credits" << endl;
		m.menu_logged_in();
		switch (choice) {
		case 1: {
			string key;
			cout << "Key : " << endl;
			cin >> key;
			int l_result = m.login(key);
			switch (l_result) {
			case 0: {
				cout << "Invalid whitelist! Please try again." << endl;
				cin;
				main();
			}
			case 1: {
				json j;
				cout << "Successfully whitelisted! Welcome, buyer!" << endl;
				j["auth"] = key;
				ofstream o;
				o.open("auth.json");
				o << j;
				o.close();
				Sleep(3);
				system("cls");
				main();
			}
			case 2: {
				json j;
				cout << "Detected an admin whitelist! Hello, Admin!" << endl;
				j["auth"] = key;
				ofstream o;
				o.open("auth.json");
				o << j;
				o.close();
				Sleep(3);
				system("cls");
				main();
			}
			case 3: {
				m.timeout_elapsed();
			}
			}
		}
		case 2: {
			m.Credit_prompt();
		}
		}
	}

	}
	return 0;
};
