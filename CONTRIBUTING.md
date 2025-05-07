# Welcome to SCuM SDK contributing guide

Thank you for contributing to the SCuM project!

We accept any type of contributions in the SDK, the nRF programmer firmware, and
the Python programmer script, but also simple edits in READMEs or bug reports.

In this guide you will get an overview of the contribution workflow from
opening an issue, creating a PR, reviewing, and merging the PR.

## Prerequisites

To get an overview of the project, read the [README](../README.md) file.
Here are some resources to help you get started to Git and the GitHub workflow:

- [Set up Git](https://docs.github.com/en/get-started/git-basics/set-up-git)
- [GitHub flow](https://docs.github.com/en/get-started/using-github/github-flow)
- [Collaborating with pull requests](https://docs.github.com/en/github/collaborating-with-pull-requests)

## Issues

### Create a new issue

If you spot a problem in the codebase, search if an issue already exists.
If a related issue doesn't exist, you can open a new issue using the
[issue form](https://github.com/pisterlab/scum-sdk/issues/new).

### Solve an issue

Scan through our [existing issues](https://github.com/pisterlab/scum-sdk/issues)
to find one that interests you.
As a general rule, we don’t assign issues to anyone. If you find an issue to
work on, you are welcome to open a PR with a fix.

## Make Changes

### Setup your fork

First, you have to [fork the repository](https://docs.github.com/en/pull-requests/collaborating-with-pull-requests/working-with-forks/fork-a-repo#forking-a-repository)

Before starting a new change, make sure your local `main` branch is in sync
with the upstream repository:
```
git switch main
git pull upstream main
```

Then create a working branch:
```
git switch -c <working branch name>
```

Avoid working directly on the `main` branch but create a working branch and
start with your changes!

### Commit your update

Commit the changes once you are happy with them. Make sure your commit message
is short and descriptive enough. This
[blog post](https://www.freecodecamp.org/news/how-to-write-better-git-commit-messages/)
is a good read in that regard.

Also self-review your changes using for example `git diff main` before moving
to the next step.

### Pull Request

When you're finished and happy with the changes, create a pull request, also
known as a PR.
- If your PR is related to existing issue, don't forget to
  [link it to that issue](https://docs.github.com/en/issues/tracking-your-work-with-issues/linking-a-pull-request-to-an-issue).
- Enable the checkbox to [allow maintainer edits](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/allowing-changes-to-a-pull-request-branch-created-from-a-fork)
  so the branch can be updated for a merge.

Once you submit your PR, a team member will review your proposal. We may ask
questions or request additional information.
- We may ask for changes to be made before a PR can be merged, either using
  [suggested changes](https://docs.github.com/en/github/collaborating-with-issues-and-pull-requests/incorporating-feedback-in-your-pull-request)
  or pull request comments. You can apply suggested changes directly through the
  GitHub web UI. You can make any other changes in your fork, then commit them
  to your branch.
- If you run into any merge issues, checkout this
  [git tutorial](https://github.com/skills/resolve-merge-conflicts) to help you
  resolve merge conflicts and other issues.
