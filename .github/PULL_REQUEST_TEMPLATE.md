
## Pull Request template

Describe the big picture of your changes here to communicate to the maintainers why we should accept this pull request. If it fixes a bug or resolves a feature request, be sure to link to that issue.


1. Make sure that your PR is not a duplicate.
2. If not, then make sure that:

    a. You have done your changes in a separate branch. Branches MUST have descriptive names that start with either the `fix/` or `feature/` prefixes. Good examples are: `fix/signin-issue` or `feature/issue-templates`.

    b. You have a descriptive commit message with a short title (first line).

    c. You have only one commit (if not, squash them into one commit).

    d. `test` doesn't throw any error. If it does, fix them first and amend your commit (`git commit --amend`).

3. **After** these steps, you're ready to open a pull request.

    a. Your pull request MUST NOT target the `master` branch on this repository. You probably want to target `staging` instead.

    b. Give a descriptive title to your PR.

    c. Provide a description of your changes.

    d. Put `closes #XXXX` in your comment to auto-close the issue that your PR fixes (if such).

IMPORTANT: Please review the [CONTRIBUTING.md](../docs/CONTRIBUTING.md) file for detailed contributing guidelines.

## Types of changes

What types of changes does your code introduce?
_Put an `x` in the boxes that apply_

- [ ] Bugfix (non-breaking change which fixes an issue)
- [ ] New feature (non-breaking change which adds functionality)
- [ ] Breaking change (fix or feature that would cause existing functionality to not work as expected)
- [ ] Documentation Update (if none of the other choices apply)

## Checklist

_Put an `x` in the boxes that apply. You can also fill these out after creating the PR. If you're unsure about any of them, don't hesitate to ask. We're here to help! This is simply a reminder of what we are going to look for before merging your code._

- [ ] I have read the [CONTRIBUTING](../docs/CONTRIBUTING.md) document
- [ ] Lint and unit tests pass locally with my changes
- [ ] I have added tests that prove my fix is effective or that my feature works
- [ ] I have added necessary documentation (if appropriate)
- [ ] Any dependent changes have been merged and published in downstream modules

## Further comments

If this is a relatively large or complex change, kick off the discussion by explaining why you chose the solution you did and what alternatives you considered, etc...

