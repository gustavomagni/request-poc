/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "request.h"
#include "fake_cpp11.h"
#include "options.h"

#include <proton/connection.hpp>
#include <proton/container.hpp>
#include <proton/delivery.hpp>
#include <proton/message.hpp>
#include <proton/messaging_handler.hpp>
#include <proton/receiver_options.hpp>
#include <proton/source_options.hpp>
#include <proton/tracker.hpp>

int main(int argc, char** argv) {
	std::string url("127.0.0.1:5672/examples");
	example::options opts(argc, argv);
	opts.add_value(url, 'a', "address", "connect and send to URL", "URL");

	try {
		opts.parse();

		std::vector<std::string> requests;
		requests.push_back("Twas brillig, and the slithy toves");
		requests.push_back("Did gire and gymble in the wabe.");
		requests.push_back("All mimsy were the borogroves,");
		requests.push_back("And the mome raths outgrabe.");

		request c(url, requests);

		proton::container(c).run();
		return 0;
	}
	catch (const example::bad_option & e) {
		std::cout << opts << std::endl << e.what() << std::endl;
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
	}
	return 1;
}

