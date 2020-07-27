---
name: Promote Changeset To Production
about: Promote a Changeset to Production
title: 'Promote Changeset To Production'
labels: 'Changeset::Production'
assignees: ''

---
### Overview Details
_______
Promotion Date: `_REPLACE_WITH_ YYYY-MM-DD`
Checklist Completer(s): `_REPLACE_WITH_ @MentionHandle(s)`
Changeset Version: `_REPLACE_WITH_ X.Y`
Stack ID for prechange Blue Stack: `_REPLACE_WITH_ STACKID`
Stack ID for prechange Green Stack: `_REPLACE_WITH_ STACKID`
Changeset Deployment Attempt Result (NO GO, SUCCEEDED, FAILED): `_REPLACE_WITH_ NO GO or SUCCEEDED or FAILED`

#### Production integration System verification
acknowledge the following checks before going into production (if applicable)
* [ ] is there a need for a new api version ? (router-augmentor breaking changes)
* [ ] were Unit tests provided to cover the new feature / bug ?
* [ ] were tests done in test environment ?
* [ ] is there a data api change (DS data schema change)