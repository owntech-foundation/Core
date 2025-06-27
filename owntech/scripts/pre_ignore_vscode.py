"""Mark VSCode settings as assume-unchanged with minimal overhead."""

from pathlib import Path
import subprocess


def main() -> None:
    settings = Path(".vscode") / "settings.json"
    flag = Path(".pio") / "ignore_vscode.flag"

    if not settings.is_file() or flag.exists():
        return

    subprocess.run(
        ["git", "update-index", "--assume-unchanged", str(settings)], check=False
    )

    flag.parent.mkdir(parents=True, exist_ok=True)
    flag.write_text("done")


if __name__ == "__main__":
    main()


