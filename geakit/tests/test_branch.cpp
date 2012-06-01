#include <iostream>
#include <git2.h>

using namespace std;

int main(int argc, char* argv[]) {
  const char* path = "/home/yuping/yp/git/geakit";
  int error;
  git_repository* m_repos;
  error = git_repository_open(&m_repos, path);
  if (error << GIT_SUCCESS) {
    cerr << "error open the repos" << endl;
    return 0;
  }
  git_reference* head;
  git_reference* newBranch;
  git_commit*  commit;
  error = git_repository_head(&head, m_repos);
  const char* name = "branch1";
  const git_oid* head_oid = git_reference_oid(head);
  error = git_commit_lookup(&commit, m_repos, head_oid);
  if (error < GIT_SUCCESS) {
    cout << "error commit" << endl;
  }
  cout << git_commit_message(commit);
  git_oid oid_out;
  git_oid_cpy(&oid_out, head_oid);
  char my_oid[41] = {0};
  git_oid_fmt(my_oid, &oid_out);
  cout << "oid is " << my_oid << endl;
  git_reference_free(head);
  error = git_reference_create_oid(&newBranch, m_repos, name, &oid_out, 1);
  if (error < GIT_SUCCESS) {
    cerr << "error" << endl;
  }
//  git_repository_free(m_repos);
 // git_reference_free(head);
 // git_reference_free(newBranch);
  git_repository_free(m_repos);
  git_commit_free(commit);
  return 0;
} 

