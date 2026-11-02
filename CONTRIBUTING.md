# Contributing

Thanks for your interest. This is an educational project, so clear code and clear
explanations matter more than features.

## Before you start

- Read the [README](README.md) and [CLAUDE.md](CLAUDE.md). CLAUDE.md lists the hard rules
  (C++26 only, Windows x64 only, no evasion features).
- Read the [Code of Conduct](CODE_OF_CONDUCT.md). It applies to all project spaces.
- For larger changes, open an issue first so the approach can be agreed before you write code.

## Development setup

See the Build section of the [README](README.md). The short version:

```sh
./build.sh debug
```

This must succeed before you open a pull request.

## Coding rules

- C++26 only. Use modern features (`std::expected`, `std::print`, `std::span`, RAII, `enum class`).
- No raw `new`/`delete`, no C-style casts, no evasion or anti-detection features.
- Keep changes focused. Do not reformat unrelated files.
- Comments explain *why* when the reason is not obvious. Do not narrate the code.
- Errors are returned with `std::expected` and carry a stage. Do not swallow errors.

## Commits and branches

- Work on a branch, not on `main`: `feat/...`, `fix/...`, `docs/...`, or `chore/...`.
- Write commit subjects in the imperative mood, under 72 characters.
- One logical change per commit.

## Pull requests

1. Fork `hasirciogluhq/CS2-Cheat-Loader` on GitHub, then clone your fork and add the
   original repository as `upstream`:

   ```sh
   git remote add upstream https://github.com/hasirciogluhq/CS2-Cheat-Loader.git
   ```

2. Create a branch from the latest `upstream/main`:

   ```sh
   git fetch upstream && git checkout -b feat/my-change upstream/main
   ```

3. Make sure `./build.sh debug` passes.
4. Push the branch to your fork and open a pull request against `hasirciogluhq/CS2-Cheat-Loader`, branch `main`.
5. Fill in the pull request template. Describe what changed, why, and how you tested it.
   If you could not test something (for example on Windows or MSVC), say so.
6. Keep the pull request small enough to review in one sitting.

A maintainer will review the change. Expect questions and requested changes. This is normal.

## Reporting bugs

Open an issue with:

- what you did, what you expected, and what happened
- the build command and the mode (`debug` or `release`)
- Windows version and whether you ran under Wine/CrossOver
- the full error output, including the `Stage` and Win32 code if the injection API printed one

## Security issues

Do not open a public issue for a vulnerability. See [SECURITY.md](SECURITY.md).

## License of contributions

By submitting a contribution you agree that it is licensed under the
[Apache License 2.0](LICENSE), the same as the project.
