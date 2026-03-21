# DevOps BootCamp: Bash SED/AWK

>Please use branch ```task_sedawk``` for this task that already exist in your forked repository after you has been started task

## Task


With given passwd file do following:

  a. Create copy of `passwd` file to `passwd_new`. **Do all modifications in `passwd_new` file**
  
  b. Change shell for user `saned` from `/usr/sbin/nologin` to `/bin/bash` using **AWK**

  c. Change shell for user `avahi` from `/usr/sbin/nologin` to `/bin/bash` using **SED**

  d. Save only 1-st 3-th 5-th 7-th columns of each string based on `:` delimiter 

  e. Remove all lines from file containing word `daemon`

  f. Change shell for all users with **even** `UID` to `/bin/zsh`

  g. `passwd_new` should not have a new line at the end of the file

>**Make sure you are not using any additional installed packages (like `gawk` and etc.)**