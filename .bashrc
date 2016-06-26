function parse_git_branch () {
     git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ (\1)/'
}

export PS1="${debian_chroot:+($debian_chroot)}\u@\h:\w[\A]\$(parse_git_branch)\$ "

