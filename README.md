# bit

**B**lockchain g**it** is a proof of concept for a version control system built on a blockchain structure.

> Notice: This project is an experiment in version control systems, exploring how blockchain concepts could lead to a more robust and secure system. It is not intended to replace existing VCS solutions yet but to serve as a proof of concept and potential foundation for future innovations.

Each commit is treated as a block in the chain, containing hashes of diffs and the references to parent commits.
It ensures the repository history is immutable, preventing unnoticable malicious modification through historical commits.

[SHA256 Implementation](https://github.com/ilvn/SHA256)
# Concept

**With high regards to [Martin](https://github.com/wmartinmimi)**

- Immutability

  The hash calculation of each commit depends on the hash of its parents, tightly coupling the exact changes commit depends on.
  
  Any attempts to change a parent commit invalidates the parent's hash, and thus affecting the hash of the child commits due to the coupling, creating a chain effect of modified hashes.

  This allows a clear detection of any modification of historical commits, even with just a recent child commit, by detecting checking any changes in the most recent shared child.

- Decentralization

  People who cloned the repository will also have a copy of the blockchain,
  allowing people to validate commits and ensure they belong to the original repository in a distributed network.

# Disadvantages
- Performance
  As a trade-off to immutability, any rebasing of large amount of commits will result in high amount of hash calculation.

  This may not affect small projects as heavily, but for large project with intensive history management and policy, rehashing may be too computational intensive.

# Usage
```
bit create
bit verify
git diff | bit add # or your way to generate diff
```

# Building
You will need to run these with elevated privilages.
```
$ make
# make install
```

# Contributions
Contributions are welcomed, feel free to open a pull request.

# License
This project is licensed under the GNU Public License v3.0. See [LICENSE](https://github.com/night0721/bit/blob/master/LICENSE) for more information.
