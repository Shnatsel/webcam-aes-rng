webcam-aes-rng
==============

Proof-of-concept random number generator that uses a webcam as the primary source of entropy. 

It runs the data aquired from the webcam through AES-CBC with cryptographically strong random key and initial vector. The algorithm is the same as http://nullprogram.com/blog/2009/01/16/ but with webcam signal instead of monotone counter.
