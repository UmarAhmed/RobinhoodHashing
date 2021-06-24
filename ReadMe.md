
This is an implementation of Robin Hood hashing based on Pedro Celis's 1984 thesis

Summary:
- Open addressing scheme (no linked lists, just a flat array)
  - Flat array makes it cache friendly and lowers memory overhead
  - Like most open addressing schemes, it has constant probe sequence length
- When inserting, we minimize how far an element is placed from the hashed index
  - While probing for an empty slot, we swap elements if its PSL is greater than the distance from the hashed index
  - This minimizes the variance of the expected and maximum probe sequences
- When removing we use the "backwards shift" method

Please see the thesis for more details: https://cs.uwaterloo.ca/research/tr/1986/CS-86-14.pdf

