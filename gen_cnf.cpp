#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include <functional>
#include <sstream>
#include <algorithm>

using namespace std;

void recursive_comb(int *indexes, int s, int rest, function<void(int *)> f) {
  if (rest == 0) {
    f(indexes);
  } else {
    if (s < 0) return;
    recursive_comb(indexes, s - 1, rest, f);
    indexes[rest - 1] = s;
    recursive_comb(indexes, s - 1, rest - 1, f);
  }
}

void foreach_comb(int n, int k, function<void(int *)> f) {
  int indexes[k];
  recursive_comb(indexes, n - 1, k, f);
}

int cardinality_am1(int &nvars, int &nclauses, vector<int> vVars, ofstream &fcnf) {
  while(vVars.size() > 1) {
    int k = 0;
    for(int j = 0; j < vVars.size()/2; j++) {
      nvars++;
      fcnf << "-" << vVars[2*j] << " ";
      fcnf << "-" << vVars[2*j+1] << " ";
      fcnf << 0 << endl;
      nclauses++;
      fcnf << "-" << nvars << " ";
      fcnf << vVars[2*j] << " ";
      fcnf << vVars[2*j+1] << " ";
      fcnf << 0 << endl;
      nclauses++;
      fcnf << nvars << " ";
      fcnf << "-" << vVars[2*j] << " ";
      fcnf << 0 << endl;
      nclauses++;
      fcnf << nvars << " ";
      fcnf << "-" << vVars[2*j+1] << " ";
      fcnf << 0 << endl;
      nclauses++;
      vVars[k++] = nvars;
    }
    if(vVars.size()%2) {
      vVars[k++] = vVars.back();
    }
    vVars.resize(k);
  }
  return 0;
}

pair<int, int> gen_cnf(ofstream &f, int num_data, int num_node, int num_cycle, int num_com, vector<vector<int> > &init_assign, vector<pair<int, int> > &coms, vector<vector<int> > &coming, vector<vector<int> > &fin_assign, vector<string> &options) {
  int num_var = 0;
  int num_cla = 0;

  int num_x = num_cycle * num_data * num_node;
  int num_y = (num_cycle - 1) * num_data * num_com;
  num_var = num_x + num_y;
    
  //initial assignment
  for(int j = 0; j < num_node; j++) {
    for(int i = 0; i < num_data; i++) {
      if (find(init_assign[j].begin(), init_assign[j].end(), i) == init_assign[j].end()) {
	f << "-" << i + j * num_data + 1 << " 0" << endl;
      }
      else{
	f << i + j * num_data + 1 << " 0" << endl;
      }
      num_cla++;
    }
  }

  //communication data ready
  for(int k = 0; k < num_cycle - 1; k++) {
    for(int h = 0; h < num_com; h++) {
      for(int i = 0; i < num_data; i++) {
	f << "-" << num_x + k * num_com * num_data + h * num_data + i + 1 << " ";
	f << k * num_node * num_data + coms[h].first * num_data + i + 1 << " 0" << endl;
	num_cla++;
      }
    }
  }
  
  //data appearance -> communication
  for(int k = 1; k < num_cycle; k++) {
    for(int j = 0; j < num_node; j++) {
      for(int i = 0; i < num_data; i++) {
	f << "-" << k * num_node * num_data + j * num_data + i + 1 << " ";
	f << (k - 1) * num_node * num_data + j * num_data + i + 1 << " ";
	for(int h : coming[j]) {
	  f << num_x + (k - 1) * num_com * num_data + h * num_data + i + 1 << " ";
	}
	f << 0 << endl;
	num_cla++;
      }
    }
  }

  //simplex
  if(find(options.begin(), options.end(), "onehot_spx_in") == options.end()) {
    for(int k = 0; k < num_cycle - 1; k++) {
      for(int h = 0; h < num_com; h++) {
	vector<int> vVars;
	for(int i = 0; i < num_data; i++) {
	  vVars.push_back(num_x + k * num_com * num_data + h * num_data + i + 1);
	}
	cardinality_am1(num_var, num_cla, vVars, f);
      }
    }
  }

  //final assignment
  for(int j = 0; j < num_node; j++) {
    for(int i = 0; i< num_data; i++) {
      if (find(fin_assign[j].begin(), fin_assign[j].end(), i) != fin_assign[j].end()) {
	if(find(options.begin(), options.end(), "imp_reg") == options.end()) {
	  f << (num_cycle - 1) * num_node * num_data + j * num_data + i + 1 << " 0" << endl;
	  num_cla++;
	}
	else {
	  for(int k = 0; k < num_cycle; k++) {
	    f << k * num_node * num_data + j * num_data + i + 1 << " ";
	  }
	  f << 0 << endl;
	  num_cla++;
	}
      }
    }
  }

  //onehot spx in
  //com i from j to k at l
  if(find(options.begin(), options.end(), "onehot_spx_in") != options.end()) {
    for(int k = 0; k < num_cycle - 1; k++) {
      for(int j = 0; j < num_node; j++) {
	vector<int> vVars;
	for(int h : coming[j]) {
	  for(int i = 0; i < num_data; i++) {
	    vVars.push_back(num_x + k * num_com * num_data + h * num_data + i + 1);
	  }
	}
	cardinality_am1(num_var, num_cla, vVars, f);
      }
    }
  }

  //systolic
  if(find(options.begin(), options.end(), "systolic") != options.end()) {
    //onehot data in node per cycle
    for(int k = 0; k < num_cycle; k++) {
      for(int j = 0; j < num_node; j++) {
	vector<int> vVars;	
	for(int i = 0; i < num_data; i++) {
	  vVars.push_back(k * num_node * num_data + j * num_data + i + 1);
	}
	cardinality_am1(num_var, num_cla, vVars, f);
      }
    }
  }

  return make_pair(num_var, num_cla);
  /*
  //onehot spx out
  //com i from j to k at l
  if(find(options.begin(), options.end(), "onehot_spx_out") != options.end()) {
    vector<vector<int> > onehot_groups(num_node * num_cycle, vector<int>(0));
    for(int l = 0; l < num_cycle; l++) {
      int count = 0;
      for(int k = 0; k < num_node; k++) {
	for(int j = 0; j < num_node; j++) {
	  if(num_spx[j][k] > 0) {
	    for(int i = 0; i < num_data; i++) {
	      onehot_groups[j + l * num_node].push_back(i + count * num_data + l * num_con * num_data + num_assign);
	    }
	    count += 1;
	  }
	}
      }
    }
    for(auto onehot_group : onehot_groups) {
      for(int i0 = 0; i0 < (int)onehot_group.size(); i0++) {
	for(int i1 = i0 + 1; i1 < (int)onehot_group.size(); i1++) {
	  Glucose::Lit l0 = Glucose::mkLit(onehot_group[i0], true);
	  Glucose::Lit l1 = Glucose::mkLit(onehot_group[i1], true);
	  S.addClause(l0, l1);
	  num_cla += 1;
	}
      }
    }
  }



  //symmetric
  //com i from j to k at l
  if(find(options.begin(), options.end(), "symmetric") != options.end()) {
    vector<int> com_types;
    for(int k = 0; k < num_node; k++) {
      for(int j = 0; j < num_node; j++) {
	if(num_spx[j][k] > 0) {
	  if(find(com_types.begin(), com_types.end(), k - j) == com_types.end()) {
	    com_types.push_back(k - j);
	  }
	  int com_type = find(com_types.begin(), com_types.end(), k - j) - com_types.begin();
	}
      }
    }
    num_lit += num_cycle * (int)com_types.size();
    while(num_lit >= S.nDatas()) S.newData();
    //communicaion direction definition
    for(int l = 0; l < num_cycle; l++) {
      int count = 0;
      for(int k = 0; k < num_node; k++) {
	for(int j = 0; j < num_node; j++) {
	  if(num_spx[j][k] > 0) {
	    int com_type = find(com_types.begin(), com_types.end(), k - j) - com_types.begin();
	    for(int i = 0; i < num_data; i++) {
	      Glucose::Lit l0 = Glucose::mkLit(i + count * num_data + l * num_con * num_data + num_assign, true);
	      Glucose::Lit l1 = Glucose::mkLit(com_type + l * (int)com_types.size() + num_com + num_assign);
	      S.addClause(l0, l1);
	    }
	    count += 1;
	  }
	}
      }
    }
    //direction must be onehot
    num_cla += num_cycle * num_con * num_data;
    for(int i0 = 0; i0 < (int)com_types.size(); i0++) {
      for(int i1 = i0 + 1; i1 < (int)com_types.size(); i1++) {
	for(int l = 0; l < num_cycle; l++) {
	  Glucose::Lit l0 = Glucose::mkLit(i0 + l * (int)com_types.size() + num_com + num_assign, true);
	  Glucose::Lit l1 = Glucose::mkLit(i1 + l * (int)com_types.size() + num_com + num_assign, true);
	  S.addClause(l0, l1);
	  num_cla += 1;
	}
      }
    }
    //send data if it has data
    for(int l = 0; l < num_cycle; l++) {    
      int count = 0;
      for(int k = 0; k < num_node; k++) {
	for(int j = 0; j < num_node; j++) {
	  if(num_spx[j][k] > 0) {
	    int com_type = find(com_types.begin(), com_types.end(), k - j) - com_types.begin();
	    Glucose::vec<Glucose::Lit> ls;
	    ls.push(Glucose::mkLit(com_type + l * (int)com_types.size() + num_com + num_assign, true));
	    for(int i = 0; i < num_data; i++) {
	      ls.push(Glucose::mkLit(i + count * num_data + l * num_con * num_data + num_assign));
	    }
	    for(int i = 0; i < num_data; i++) {
	      ls.push(Glucose::mkLit(i + j * num_data + l * num_node * num_data, true));
	      S.addClause(ls);
	      ls.pop();
	    }
	    count += 1;
	  }
	}
      }
    }
    num_cla += num_cycle * num_con * num_data;
    //communication -> data appearance (no redundant communication in which received data is not stored)
    for(int l = 0; l < num_cycle; l++) {
      int count = 0;
      for(int k = 0; k < num_node; k++) {
	for(int j = 0; j < num_node; j++) {
	  if(num_spx[j][k] > 0) {
	    for(int i = 0; i < num_data; i++) {
	      Glucose::Lit lcom = Glucose::mkLit(i + count * num_data + l * num_con * num_data + num_assign);
	      Glucose::Lit ldprev = Glucose::mkLit(i + k * num_data + l * num_node * num_data);
	      Glucose::Lit ldnext = Glucose::mkLit(i + k * num_data + (l + 1) * num_node * num_data);
	      S.addClause(~lcom, ~ldprev);
	      S.addClause(~lcom, ldnext);
	    }
	    count += 1;
	  }
	}
      }
    }
    num_cla += num_cycle * num_con * num_data * 2;
    //data cannot remain in the same node
    for(int l = 0; l < num_cycle; l++) {
      for(int k = 0; k < num_node; k++) {
	for(int i = 0; i < num_data; i++) {
	  Glucose::Lit l0 = Glucose::mkLit(i + k * num_data + l * num_node * num_data, true);
	  Glucose::Lit l1 = Glucose::mkLit(i + k * num_data + (l + 1) * num_node * num_data, true);
	  S.addClause(l0, l1);
	}
      }
    }
    num_cla += num_cycle * num_node * num_data;
  }
  
  //  f << "p cnf " << num_lit << " " << num_cla << "\n";
  */
}

string text_omit_int(vector<vector<int> > array) {
  string str = "";
  for(vector<int> vec : array) {
    str += "[";
    for(int elem : vec) {
      str += to_string(elem) + ",";
    }
    str += "]";
    if(str.length() > 20) {
      str += "...";
      break;
    }
  }
  return str;
}


		  
string text_omit(vector<vector<string> > array) {
  string str = "";
  for(vector<string> vec : array) {
    str += "[";
    for(string elem : vec) {
      str += elem + ",";
    }
    str += "]";
    if(str.length() > 20) {
      str += "...";
      break;
    }
  }
  return str;
}
 
int main(int argc, char ** argv) {
  if(argc < 2) {
    cout << "setting file should be given as second command line parameter" << endl;
    return 1;
  }
  
  int fverbose = 0;
  if(argc > 2) {
    fverbose = 1;
  }

  string setting_file_name = string(argv[1]);
  ifstream setting_file(setting_file_name);
  if (!setting_file) {
    cerr << "setting file should be given as second command line parameter" << endl;
    cerr << "setting file cannot be open" << endl;
    return 1;
  }
  
  int num_cycle = 0;
  int num_node = 0;
  vector<vector<int> > num_spx;
  vector<vector<string> > init_assign, fin_assign;
  vector<string> options;

  string str;
  while (getline(setting_file, str)) {
    if(str == "cycle") {
      getline(setting_file, str);
      num_cycle = stoi(str);
    }
    else if(str == "node") {
      getline(setting_file, str);
      num_node = stoi(str);
    }
    else if(str == "spx") {
      if(!num_spx.empty()) {
	cout <<  "spx is declared more than once" << endl;
	return 1;
      }
      if(num_node == 0 ) {
	cout <<  "write num node before spx" << endl;	
	return 1;
      }
      for(int i = 0; i < num_node; i++) {
	vector<int> num_spx_i;
	getline(setting_file, str);
	char delim = ' ';
	stringstream ss(str);
	string str2;
	while(getline(ss, str2, delim)) {
	  num_spx_i.push_back(stoi(str2));
	}
	num_spx.push_back(num_spx_i);
      }
    }
    else if(str == "initial") {
      if(!init_assign.empty()) {
	cout <<  "initial assignment is declared more than once" << endl;
	return 1;
      }
      if(num_node == 0) {
	cout <<  "write num node before initial assignment" << endl;	
	return 1;
      }
      for(int i = 0; i < num_node; i++) {
	vector<string> initial_i;
	getline(setting_file, str);
	char delim = ' ';
	stringstream ss(str);
	string str2;
	while(getline(ss, str2, delim)) {
	  initial_i.push_back(str2);
	}
	init_assign.push_back(initial_i);
      }
    }
    else if(str == "final") {
      if(!fin_assign.empty()) {
	cout <<  "final assignment is declared more than once" << endl;
	return 1;
      }
      if(num_node == 0 ) {
	cout <<  "write num node before final assignment" << endl;	
	return 1;
      }
      for(int i = 0; i < num_node; i++) {
	vector<string> final_i;
	getline(setting_file, str);
	char delim = ' ';
	stringstream ss(str);
	string str2;
	while(getline(ss, str2, delim)) {
	  final_i.push_back(str2);
	}
	fin_assign.push_back(final_i);
      }
    }
    else if(str == "onehot_spx_in") {
      options.push_back(str);
    }
    else if(str == "onehot_spx_out") {
      options.push_back(str);
    }
    else if(str == "implicit_reg") {
      options.push_back("imp_reg");
    }
    else if(str == "systolic") {
      options.push_back(str);
      options.push_back("imp_reg");
    }
    else if(str == "symmetric") {
      options.push_back(str);
    }
  }

  /*
  cout << "setting file:" << setting_file_name << endl;
  cout << "num_cycle: " << num_cycle << endl;
  cout << "num_node: " << num_node << endl;
  cout << "num_spx: " << text_omit_int(num_spx) << endl;
  cout << "initial assignment: " << text_omit(init_assign) << endl;
  cout << "final assignment: " << text_omit(fin_assign) << endl;
  for(auto option : options) {
    cout << option << endl;
  }
  */
  setting_file.close();
  
  int num_data = 0;
  map<string, int> data_id;
  map<int, string> id_data;
  vector<vector<int> > init_assign_id, fin_assign_id;
  
  for(vector<string> row : init_assign) {
    vector<int> init_assign_id_row;
    for(string elem : row) {
      if (data_id.find(elem) == data_id.end()){
	data_id[elem] = num_data;
	id_data[num_data] = elem;
	num_data += 1;
      }
      init_assign_id_row.push_back(data_id[elem]);
    }
    init_assign_id.push_back(init_assign_id_row);
  }

  for(vector<string> row : fin_assign) {
    vector<int> fin_assign_id_row;
    for(string elem : row) {
      fin_assign_id_row.push_back(data_id[elem]);
    }
    fin_assign_id.push_back(fin_assign_id_row);
  }

  /*
  cout << "num_data: " << num_data << endl;
  cout << "initial assignment: " << text_omit_int(init_assign_id) << endl;
  cout << "final assignment: " << text_omit_int(fin_assign_id) << endl;
  */
  
  int num_com = 0;
  vector<pair<int ,int> > coms;
  vector<vector<int> > coming;
  coming.resize(num_node);
  for(int i = 0; i < num_node; i++) {
    for(int j = 0; j < num_node; j++) {
      for(int k = 0; k < num_spx[i][j]; k++) {
	coms.push_back(make_pair(i, j));
	coming[j].push_back(num_com);
	num_com++;
      }
    }
  }

  /*
  cout << "num_com: " << num_com << endl;
  cout << "communication paths: ";
  for(auto com : coms) {
    cout << "(" << com.first << ", " << com.second << "), ";
  }
  cout << endl;
  */
  
  string cnf_file_name = "_" + setting_file_name + ".cnf";
  ofstream cnf_file(cnf_file_name);
  if(!cnf_file) {
    cerr << "cnf_file(_[setting file].cnf) cannot be open" << endl;
    return 1;
  }

  auto cnf_stats = gen_cnf(cnf_file, num_data, num_node, num_cycle, num_com, init_assign_id, coms, coming, fin_assign_id, options);

  cnf_file.close();

  string cnf_header = "p cnf " + to_string(cnf_stats.first) + " " + to_string(cnf_stats.second);
  string cnf_mod_cmd = "sed -i \'1i" + cnf_header + "\' " + cnf_file_name;
  system(cnf_mod_cmd.c_str());

  string res_file_name = "_" + setting_file_name + ".out";
  string log_file_name = "_" + setting_file_name + ".log";
  //  string sat_cmd = "minisat " + cnf_file_name + " " + res_file_name + " > " + log_file_name;
  string sat_cmd = "plingeling " + cnf_file_name + " > " + res_file_name;
  system(sat_cmd.c_str());

  string log_cmd = "echo \"" + cnf_header + "\" >> " + log_file_name;
  system(log_cmd.c_str());
  
  vector<int> results;
  ifstream res_file(res_file_name);
  if(!res_file) {
    cout << "cannot open results file" << endl;
    return 1;
  }

  while(getline(res_file, str)) {
    string s;
    stringstream ss(str);
    vector<string> vs;
    getline(ss, s, ' ');
    if(s == "1" || s == "-1") {
      results.push_back(stoi(s));
      while(getline(ss, s, ' ')) {
	results.push_back(stoi(s));
      }
    } else if(s == "v") {
      while(getline(ss, s, ' ')) {
	results.push_back(stoi(s));
      }
    }
  }

  if(results.empty()) {
    //    cout << "UNSAT" << endl;
    return 1;
  }

  int num_x = num_cycle * num_node * num_data;
  vector<vector<vector<int> > > image;
  image.resize(num_cycle);
  for(int k = 0; k < num_cycle; k++) {
    image[k].resize(num_node + num_com);
    for(int j = 0; j < num_node; j++) {
      for(int i = 0; i < num_data; i++) {
	if(results[k * num_node * num_data + j * num_data + i] > 0) {
	  image[k][j].push_back(i);
	}
      }
    }
    for(int h = 0; h < num_com; h++) {
      for(int i = 0; i < num_data; i++) {
	if(results[num_x + k * num_com * num_data + h * num_data + i] > 0) {
	  image[k][num_node + h].push_back(i);
	}
      }
    }
  }

  if(fverbose) {
    cout << "results:" << endl;
    for(int k = 0; k < num_cycle; k++) {
      cout << "\tcycle " << k + 1 << ":" << endl;
      for(int j = 0; j < num_node; j++) {
	cout << "\t\tnode " << j + 1 << ": ";
	for(int i : image[k][j]) {
	  cout << id_data[i] << ", ";
	}
	cout << endl;
      }
      if(k == num_cycle -1) {
	break;
      }
      for(int h = 0; h < num_com; h++) {
	cout << "\t\tpath " << h + 1 << "(" << coms[h].first << "->" << coms[h].second << "): ";
	for(int i : image[k][num_node + h]) {
	  cout << id_data[i] << ", ";
	}
	cout << endl;
      }
    }
  }    

  string assign_file_name = "_" + setting_file_name + ".assign";
  ofstream assign_file(assign_file_name);
  for(int k = 0; k < num_cycle; k++) {
    for(int j = 0; j < num_node; j++) {
      for(int i : image[k][j]) {
	assign_file << k << " " << j << " " << id_data[i] << endl;
      }
    }
  }
  assign_file.close();

  string com_file_name = "_" + setting_file_name + ".com";
  ofstream com_file(com_file_name);
  for(int k = 0; k < num_cycle-1; k++) {
    for(int h = 0; h < num_com; h++) {
      for(int i : image[k][num_node + h]) {
	com_file << k << " " << coms[h].first << " " << coms[h].second << " " << id_data[i] << endl;
      }
    }
  }
  com_file.close();

  return 0;
}
