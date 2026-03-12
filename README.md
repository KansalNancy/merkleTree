This is the c++ implementation of Merkle Tree which support 4 functions.

1. static size_t tree_size(size_t n)
   Returns the size of the vector in which we want to put the digests of all the nodes of the merkle tree, given data of size n.
   
2. static std::vector<Hash> build_tree(const std::vector<dataT>& data);
   Fill that vector with the hashes of merkle tree.
   
3. static std::vector<ProofNode> get_proof(const std::vector<Hash>& tree, size_t leaf_index, size_t data_size);
   Say, I am data item 2, which is the leaf index, return the proof of merkle tree to calculate root using the proof and data.
   
4. static bool verify_proof(dataT& data, const std::vector<ProofNode>& proof, const Hash& root);
   Returns true if the given proof is correct to make the correct merkle tree root.
   Make the root and compare it with the actual root.
