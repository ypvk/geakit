#include <iostream>
#include <git2.h>

using namespace std;

int show_value(const char* var_name, const char* value, void* payload) {
  cout << var_name << ": " << value << endl;
	return 0;
}

int main(int argc, char* argv[]) {
  const char* path = "/home/yuping/yp/git/new_git";
  int error;
  git_repository* m_repos;
  error = git_repository_open(&m_repos, path);
  if (error << GIT_OK) {
    cerr << "error open the repos" << endl;
    return 0;
  }
  git_config* m_config;
  error = git_repository_config(&m_config, m_repos);
  if (error < GIT_OK) {
    cerr << "error get config" << endl;
    
  }
  
  error = git_config_foreach(m_config, show_value, NULL);
  if ( error < GIT_OK) {
    cerr << "error show the config message!";
  }
  const char* value;
  const char* name = "remote.origin.url"; 
  const char* valueToSet = "http://yupingseu12345";
  error = git_config_get_string(&value, m_config, name);
  if (error == GIT_OK) {
    cout << name << ": " <<value << endl;
  }
  git_config_set_string(m_config, name, valueToSet);

  /*
  git_config* m_config1;
  error = git_config_open_ondisk(&m_config1, "/home/yuping/yp/new_git/.git11");
  if ( error < GIT_OK) {
    cout << "error open";
  }
  error = git_config_get_string(m_config1, name, &value);
  cout << "cout 1" << endl;
  if (error == GIT_OK) {
    cout << "get 2" << endl;
    cout << name << ": " <<value << endl;
  }
  git_config_free(m_config1);
  */
  git_config_free(m_config);
  git_repository_free(m_repos);
  return 0;
}
  

