#!/usr/bin/env python3
"""
Generates CHANGELOG.md from the git history: one section per tag (newest first), with the
tag date, commits grouped by Conventional Commits type, and a table of contents of the versions.
Commits after the latest tag are listed under "Unreleased".

Usage: python3 scripts/changelog.py [output file]   (default: CHANGELOG.md in the repository root)
"""

import re
import subprocess
import sys
from pathlib import Path

# Conventional Commits types, in the order they appear in each version
SECTIONS = [
    ("feat", "Features"),
    ("fix", "Bug Fixes"),
    ("perf", "Performance"),
    ("refactor", "Refactoring"),
    ("build", "Build"),
    ("test", "Tests"),
    ("docs", "Documentation"),
    ("chore", "Chores"),
]
OTHER = "Other changes"

CONVENTIONAL = re.compile(r"^(?P<type>[a-z]+)(\((?P<scope>[^)]+)\))?(?P<breaking>!)?: (?P<description>.+)$")


def git(*args):
    return subprocess.run(["git", *args], check=True, capture_output=True, text=True).stdout.strip()


def repository_url():
    """GitHub web URL of the origin remote, or None if it can't be derived."""
    try:
        url = git("remote", "get-url", "origin")
    except subprocess.CalledProcessError:
        return None

    match = re.match(r"^(?:https://|git@)github\.com[/:](?P<path>[^/]+/[^/]+?)(?:\.git)?$", url)
    return f"https://github.com/{match['path']}" if match else None


def tags():
    """Version tags as (name, date), oldest first. The date is the tagger date, or the commit date for lightweight tags."""
    lines = git("for-each-ref", "refs/tags", "--sort=creatordate", "--format=%(refname:short)|%(creatordate:short)")
    return [tuple(line.split("|")) for line in lines.splitlines() if line]


def commits(revision_range):
    """Non-merge commits of a range as (hash, subject), newest first."""
    lines = git("log", "--no-merges", "--format=%h|%s", revision_range)
    return [tuple(line.split("|", 1)) for line in lines.splitlines() if line]


def group(version_commits):
    """Groups commits by section title, keeping the SECTIONS order."""
    titles = dict(SECTIONS)
    groups = {title: [] for _, title in SECTIONS}
    groups[OTHER] = []

    for commit_hash, subject in version_commits:
        match = CONVENTIONAL.match(subject)
        if match and match["type"] in titles:
            description = match["description"]
            if match["scope"]:
                description = f"**{match['scope']}**: {description}"
            if match["breaking"]:
                description = f"**BREAKING** {description}"
            groups[titles[match["type"]]].append((commit_hash, description))
        else:
            groups[OTHER].append((commit_hash, subject))

    return [(title, entries) for title, entries in groups.items() if entries]


def render(versions, url):
    """versions: list of (name, date or None, previous tag or None, commits), newest first."""
    lines = [
        "# Changelog",
        "",
        "All notable changes of each version, generated from the git history by `scripts/changelog.py`: don't edit it by hand.",
        "",
        "## Versions",
        "",
    ]
    for name, date, _, _ in versions:
        lines.append(f"- [{name}](#{name})" + (f" ({date})" if date else ""))

    for name, date, previous, version_commits in versions:
        lines += ["", f'<a id="{name}"></a>', "", f"## {name}" + (f" ({date})" if date else ""), ""]

        if url and previous:
            target = "HEAD" if date is None else name
            lines += [f"[Compare with {previous}]({url}/compare/{previous}...{target})", ""]

        for title, entries in group(version_commits):
            lines += [f"### {title}", ""]
            for commit_hash, description in entries:
                reference = f"[`{commit_hash}`]({url}/commit/{commit_hash})" if url else f"`{commit_hash}`"
                lines.append(f"- {description} ({reference})")
            lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def main():
    root = Path(git("rev-parse", "--show-toplevel"))
    output = Path(sys.argv[1]) if len(sys.argv) > 1 else root / "CHANGELOG.md"

    versions = []
    previous = None
    for name, date in tags():
        revision_range = f"{previous}..{name}" if previous else name
        versions.append((name, date, previous, commits(revision_range)))
        previous = name

    if previous:
        unreleased = commits(f"{previous}..HEAD")
        if unreleased:
            versions.append(("Unreleased", None, previous, unreleased))

    output.write_text(render(list(reversed(versions)), repository_url()), encoding="utf-8")
    print(f"Wrote {output} ({len(versions)} versions)")


if __name__ == "__main__":
    main()
