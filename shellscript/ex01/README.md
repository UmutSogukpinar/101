# DevOps Essentials: Task 1 - Account Processing

This project is designed to automatically process company employee account records, standardize names, and generate unique email addresses following corporate rules.

---

## Scope of the Task

The `task1.sh` script processes the input file `accounts.csv` and performs the following operations:

### 1. Name Formatting

* Converts first and last names into **Title Case**.
* The first letter is capitalized, and the remaining letters are lowercase.

**Example:**

```
bart charlow -> Bart Charlow
```

---

### 2. Email Generation

* Email format:

```
<first_initial><lastname>@abc.com
```

* All characters are converted to lowercase.

**Example:**

```
Bart Charlow -> bcharlow@abc.com
```

---

### 3. Conflict Resolution

If multiple users share the same first initial and last name:

* The script appends the user's `location_id` (column 2) to the email.

**Example:**

```
bcharlow6@abc.com
bcharlow7@abc.com
```

---

### 4. Output

* The processed data is saved into a new file:

```
accounts_new.csv
```

---

## Usage

### 1. Grant execution permission

```bash
chmod +x task1.sh
```

### 2. Run the script

```bash
./task1.sh accounts.csv
```

---

## Technical Constraints & Requirements

### No External Dependencies

* The script does not require any additional packages.
* It is implemented using standard Linux tools:

  * `bash`
  * `awk`

---

### Compatibility

* Tested on standard Linux distributions.
* Works without additional configuration.

---

## Notes

* Input file must be in valid CSV format.
* Column assumptions:

  * Column 2: `location_id`
  * Column 3: `name`
  * Column 5: `email`

---

## Summary

This script ensures:

* Consistent name formatting
* Deterministic and unique email generation
* Lightweight and dependency-free execution

A practical solution for automating employee account processing in DevOps workflows.
