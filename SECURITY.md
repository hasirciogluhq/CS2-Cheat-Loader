# Security Policy

## Scope

This is an educational project. Security issues in its own code are welcome:
memory safety bugs, unchecked remote writes, unsafe handle handling, and similar
problems in the injection API or the window and renderer code.

Out of scope: using the project against software you do not own or are not
authorized to test.

## Reporting a vulnerability

Do not open a public GitHub issue for a security problem.

Use GitHub's private vulnerability reporting: go to the repository's **Security** tab
and choose **Report a vulnerability**. Include:

- the affected file and function
- steps to reproduce, or a minimal input
- the expected and actual behavior
- your Windows version and build mode

You can expect an acknowledgement within 7 days. Fixes are made on a private branch
and published with a note in the release. Please allow time for a fix before public
disclosure.

## Supported versions

Only the latest commit on `main` is supported.
