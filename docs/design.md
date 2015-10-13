# Nako Design Document

## Summary
`nako` is a simplified version control system which can be used to for everything from managing small projects by individuals to massive projects on which multiple people are working on simultaneously. Using the ever present and robust `diff` and `diff3` utilities, `nako` is fast, efficient and simple.

## Usage
`nako`'s usage is very simple.

* `nako init` initializes the repository inside the current working directory.
* `nako status` prints the curret snap hash, branch name, etc.
* `nako stage [filenames]` puts the files into the staging area, ready to be snapted. Supports regexp for filesnames.
* `nako destage [filenames]` removes these files from the staging area. Supports regexp for filesnames.
* `nako snap "Your snap message here."` takes a snapshot of  the current staged files and advances the current branch pointer to the new snap.
* `nako branch [branch name]` creates a new branch, pointing at the current snap.
* `nako switch [snap hash or branch name]` switches the current working directory to be the same as that of the snap mentioned by the hash/branch name.
* `nako merge [branch name]` merges the current branch with branch name. This uses diff.

Beyond this, there are also the following commands, which aren't really essential to using `nako`, but can be useful to more advanced users:

* `nako tag [snap hash or branch name] [tag name]` adds a tag `[tag name]` to the snap pointed at by the hash or branch name.
* `nako diff` shows the difference between the current working directory and the last snap.

## Internals

### Objects
In `nako`, each file and directory is stored in a folder `objects` inside `.nako`. Each file is simply stored as itself, just named after its SHA1 hash. All these files are compressed by `zlib`.

A folder file is basically a text file named after its SHA1 hash. The first line of this file is the full name of the folder with respect to the folder the `.nako` folder is in, so a folder `include` inside `src` inside the project parent directory would be stored as `/src/include/`. Each subsequent line is of the following format, a SHA1 hash, a whitespace, and then the name of the file or sub-directory. The names here are with respect to the parent folder, so inside the previously mentioned `include`, if there was a folder `utils`, this will be stored as simply `utils/`, while a file `utils.c` would be stored by that name alone.

### Snaps
Each `snap` is just a file inside `.nako/snaps`. The first line of this file is the snap message. The later lines are all SHA1 hashes of the contents of the parent project folder at the time of the commit. For example, if the parent folder just had the following sub-directories: `src`, 'bin', and the following files: 'makefile', 'README', then the snap file will only contain four hashes, one for `src`, one for `bin`, one for `makefile`, and one for `README`. `nako` can then construct the entire folder by just looking in objects for these files. Each snap is named after the SHA1 hash of its snap file, and this is also the name of the snap file.

### Branches
Each branch is just a file inside `nako/branches`, named after the branch name, and having a commit hash inside it.

### Tags
Inside `.nako/tags`, there are files for all the snaps, named after the snap hashes. Inside each file is a list of newline seperated tags for the commit, in no particular order. This is a seperate file from the real snapfile as tags can be editied later, but snap messages and contents can't be.
