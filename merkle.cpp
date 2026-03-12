#include<iostream>
#include <vector>
#include<string>
#include<functional>

namespace merkle{
    using Hash = std::string;
    using dataT = std::string;

    /*  A dummy hash function to gerenate the hash of the data. We can replace it with tha actual one we want to use. */
    Hash hash(const dataT& data){
        return std::to_string(std::hash<dataT>{}(data));
    }

    /* Create the hash of two nodes/siblings. */
    Hash hash(const Hash& n1, const Hash& n2){
        return Hash(n1 + n2);
    }

    /* a data structure to store proof. */
    struct ProofNode{
        Hash hash;
        bool is_left;
    };

    struct MerkleTree{
        public:
        /* Returns the size of the tree to store the hashes of all the leaf data which lets say is n. */
        static size_t tree_size(size_t n);

        /* Returns the merkle tree, which is the array of merkle nodes' hashes arranged in a tree structure such that for each node, the left child of the node
         * node is at 2 * i + 1 and right child of the node is at 2 * i + 2.
        */
        static std::vector<Hash> build_tree(const std::vector<dataT>& data);

        /* Returns the proof (a vector of proof nodes) for the given leaf_index, which is the vector of hashes of the nodes which are needed to make 
         * the root of the merkle tree.
        */
        static std::vector<ProofNode> get_proof(const std::vector<Hash>& tree, size_t leaf_index, size_t data_size);


        /* Returns true if the given proof is correct to make the correct merkle tree root. Make the root and compare it with the actual root. */
        static bool verify_proof(dataT& data, const std::vector<ProofNode>& proof, const Hash& root);
    };

}


size_t merkle::MerkleTree::tree_size(size_t n){

    /* given n data nodes for the hashes of the leaves, there would be a total of 2 * n - 1 hashes generated for all the nodes of the merkle tree. 
     * Thus, it would be the size of my vector where I want to keep my hashes of the merkle tree.
    */
    return 2 * n - 1;
}


std::vector<merkle::Hash> merkle::MerkleTree::build_tree(const std::vector<dataT>& data){
    size_t n = data.size();
    size_t tree_size = merkle::MerkleTree::tree_size(n);

    std::vector<merkle::Hash> tree(tree_size);

    size_t leaf_start_index = n - 1;
    
    /* First, we fill the hashes for the leaf nodes, which are the data nodes. */
    for(int i = 0; i < n; ++i){
        tree[leaf_start_index + i] = hash(data[i]);
    }

    /* Next, when we have the hashes of all the leaf nodes, we can calculate the hashes of the internal nodes,
     * which will be hash of 2 * i + 1 and 2 * i + 2 indexed node hash.
     * Also, note that we are starting from n - 2 and fill in reverse order as we have the hashes of leaf nodes available only yet.
    */

    for(int i = leaf_start_index - 1; i >= 0; --i){
        tree[i] = merkle::hash(tree[2 * i + 1], tree[2 * i + 2]);
    }
    return tree;
}

std::vector<merkle::ProofNode> merkle::MerkleTree::get_proof(const std::vector<Hash>& tree, size_t data_index, size_t data_size){
    std::vector<merkle::ProofNode> proof;

    /* First, convert the given data_index to the index where it is located in the tree. */
    size_t tree_index = data_index + data_size - 1;

    /* Next, extract the nodes from the tree which are a sibling of the current one and are the uncles. 
     * If, the current node is on the odd index, its sibling will be on the left, so is_left is true, 
     * otherwise, the sibling is at the right so false. Note that this is need because 
     * hash(a + b) != hash(b + a), so we have to concatenate the hashes in order they occur in the tree.
     * Once we are done taking the sibling, we go to the parent of the current and repeat the same with him
     * to extract the sibling of parent (which is the uncle of the current node).
    */
    while(tree_index > 0){
        if((tree_index & 1) == 0){
            proof.push_back({tree[tree_index - 1], true});
        }
        else{
            proof.push_back({tree[tree_index + 1], false});
        }
        tree_index = (tree_index - 1) / 2;  //can do >> 2, if division is costly you think.
    }
    return proof;
}

bool merkle::MerkleTree::verify_proof(dataT& data, const std::vector<ProofNode>& proof, const Hash& root){
    /* First, we extract the hash of the data that is shared to us. */
    Hash current_node_hash = hash(data);

    /* Next, to check if the data is valid, we calculate the root of the merkle tree using the proof that is given to us. 
     *If the new calculated root is the same as the root given by the primary, we say proof and data is valid,
     * else, something is wrong.
    */
    for(const auto& sibling_node: proof){
        if(sibling_node.is_left){
            current_node_hash = hash(sibling_node.hash, current_node_hash);
        }
        else{
            current_node_hash = hash(current_node_hash, sibling_node.hash);
        }
    }
    return current_node_hash == root;
}

/* TESTER FUNCTION */
int main(){
    std::vector<merkle::dataT> data = {"A", "B", "C", "D", "E"};
    size_t n = data.size();

    auto tree = merkle::MerkleTree::build_tree(data);
    merkle::Hash root = tree[0];

    std::cout<<root<<std::endl;

    size_t data_index = 1;  //which is "B"

    auto proof = merkle::MerkleTree::get_proof(tree, data_index, n);

    bool verified = merkle::MerkleTree::verify_proof(data[1], proof, root);

    if(verified){
        std::cout<<"Data Verified!!!!"<<std::endl;
    }
    else{
        std::cout<<"Something is wrong :("<<std::endl;
    }
}