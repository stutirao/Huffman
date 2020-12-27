all: help.cpp main.cpp param_parser.cpp mainlib/huffman.cpp mainlib/tree.cpp 
	@echo
	@echo "Building normal programs"
	$(CXX) -c help.cpp -o .help
	$(CXX) -c main.cpp -o .main
	$(CXX) -c param_parser.cpp -o .param_parser
	$(CXX) -c mainlib/huffman.cpp -o .huffman
	$(CXX) -c mainlib/huffman_decode.cpp -o .huffman_d
	$(CXX) -c mainlib/tree.cpp -o .tree
	$(CXX) -c mainlib/bitstream.cpp -o .bitstream
	$(CXX) .help .main .param_parser .huffman .tree .bitstream .huffman_d -o huff
clean:
	@echo
	@echo "Cleaning up the binaries"
	rm .bitstream .help .huff .huffman* .priority_queue .param_parser .tree .main