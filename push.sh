#!/bin/sh
setup_git() {
  git config --global user.email "travis@travis-ci.org"
  git config --global user.name "Travis CI"
}
merge_branch() {
    git config --add remote.origin.fetch +refs/heads/*:refs/remotes/origin/* || exit
    git fetch --all || exit
    git checkout release || exit
    git merge --no-ff "$TRAVIS_COMMIT" || exit
    git push @github.com/">https://${GITHUB_TOKEN}@github.com/<your-github-user>/<your-repository-name>.git
}

setup_git
merge_branch



