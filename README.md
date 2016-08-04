# SUSYUpgradeAnalysis

# Some usefull Git commands

git rm --cached `git ls-files -i -X .gitignore`
git commit -m "Cleaning"
git push origin master

git checkout -b [name_of_your_new_branch]
git push origin [name_of_your_new_branch]
git branch

git tag -a -m "Tagging release 1.0" v1.0

git checkout better_branch
git merge --strategy=ours master    # keep the content of this branch, but record a merge
git checkout master
git merge better_branch     