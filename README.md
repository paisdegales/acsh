# The anticrowd shell (acsh for short)

This project was presented as a work during my Operational Systems class. All specifications can be found in 'T1.pdf' (its in portuguese).

---

## What's acsh?

A simple shell which takes inspiration on COVID's measures and attempts to run isolated groups of commands in background.

---

## How does it work?

Acsh recognizes 2 categories of commands:
1. internal commands (cd and exit)
2. external commands (any other program)

By default acsh runs external commands in background and internal commands in foreground.

It can also run one background command in foreground when `%` is provided by the user as the last word in the commandline

Acsh recognizes three formats of user input:

1. Running one command in background

    ```acsh> command [options]```

2. Running one command in foreground

    ```acsh> command [options] %```

3. Running up to 5 commands in background

    ```acsh> command [options] <3 command [options] <3 command [options] <3 command [options] <3 command [options]```

---

## How does it work under the hood?

Acsh performs tasks in a similar way as an ordinary shell.

The way it works can be summarized as the following:
1. acsh waits patiently for the user to enter some command
2. acsh stores what was entered in a buffer and breaks it down into tokens
3. tokens are grouped into jobs (`<3` separates different jobs)
4. tokens/jobs are analyzed and executed according with their categories and running modes
5. repeat until the user get's tired and enter Ctrl-D

To accomplish what it's intended for, acsh performs many operations such as:
1. forking
2. waiting for child processes
3. changing the process' signal mask
4. signal handling
5. executing commands

---

## This is very interesting! How can I read more about this?

You can find useful informating by reading `manpages`

I recommend these ones:

```
    man 7 signal
    man 2 execve
    man 3 execvp
```

You can also take a look at bash's documentation. It teaches how a shell works (at least the basics).
