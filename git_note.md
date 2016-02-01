WORKING WITH GIT

1
git push origin head:refs/for/fpt_lpet_frdmkl33z_hal_dac
git push origin HEAD:refs/for/master
Push a branch to gerrit repository 
2.pull
git pull <remote> <branch>
If you wish to set tracking information for this branch you can do so with:
git branch --set-upstream-to=origin/<branch> develope_ble_led_button
3.
git commit --amend
Amend previous commit
4.
git remote -v
5.
git fetch origin
git pull origin [branch_name]
6.
git submodule init
git submodule update
git submodule deinit -f

7.
git vim .gitmodules
git checkout .gitmodules

8.
git clean - Remove untracked files from the working tree
git clean -n
Perform a “dry run” of git clean. This will show you which files are going to be removed without actually doing it.
git clean -f
Remove untracked files from the current directory
git clean -f <path>
Remove untracked files, but limit the operation to the specified path.
git clean -df
Remove untracked files and untracked directories from the current directory.
[-d] Remove untracked directories in addition to untracked files.
[f] --force
git clean -xf
Remove untracked files from the current directory as well as any files that Git usually ignores.

9.
Update pack set
git commit --amend
	then:
git push origin HEAD:refs/for/master

10.
gitk --all
Displays changes in a repository or a selected set of commits

11. git log
-n
	Display the n last commit

--since=<date>
--after=<date>
	Show commits more recent than a specific date.

--author=<pattern>
--committer=<pattern>
	Limit output with pattern

--merges
	Print only merge commits. This is exactly the same as --min-parents=2.
12. HEAD
First all what is HEAD?
	HEAD is a simply a reference to the current commit (latest) in the current branch.
	There can only be 1 HEAD at any given time.
	If you are not on the latest commit - meaning that HEAD is point to a prior commit in history its called detached HEAD

git checkout
git checkout <commit_id>
git checkout -b <new branch> <commit_id>
git checkout HEAD~X // x is the number of commits t go back
	This will checkout new branch pointing to the desired commit.
	This command will checkout to a given commit.
	At this point you can create a branch and start to work from this point on

git reflog
git reflog
git checkout HEAD@{...}
This will get you back to your desired commit


13. diff
git diff [--options] <commit> <commit> [--] [<path>...]
	Show changes between commits, commit and working tree, etc

git diff HEAD^^ HEAD main.c
	to see the difference for a file "main.c