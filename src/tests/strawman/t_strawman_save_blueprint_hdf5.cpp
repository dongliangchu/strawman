//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2015-2017, Lawrence Livermore National Security, LLC.
// 
// Produced at the Lawrence Livermore National Laboratory
// 
// LLNL-CODE-716457
// 
// All rights reserved.
// 
// This file is part of Strawman. 
// 
// For details, see: http://software.llnl.gov/strawman/.
// 
// Please also read strawman/LICENSE
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the disclaimer below.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the disclaimer (as noted below) in the
//   documentation and/or other materials provided with the distribution.
// 
// * Neither the name of the LLNS/LLNL nor the names of its contributors may
//   be used to endorse or promote products derived from this software without
//   specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL SECURITY,
// LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
// DAMAGES  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.
// 
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

//-----------------------------------------------------------------------------
///
/// file: t_strawman_save_hdf5.cpp
///
//-----------------------------------------------------------------------------

#include "gtest/gtest.h"

#include <strawman.hpp>

#include <iostream>
#include <math.h>
#include <sstream>
#include <mpi.h>
#include <conduit_blueprint.hpp>

#include "t_config.hpp"
#include "t_strawman_test_utils.hpp"


using namespace std;
using namespace conduit;
using namespace strawman;


//-----------------------------------------------------------------------------
TEST(strawman_test_2d_hdf5, test_2d_serial_hdf5_pipeline)
{
    //
    // Create example mesh.
    //
    int par_rank;
    int par_size;
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &par_rank);
    MPI_Comm_size(comm, &par_size);

    Node data, verify_info;
    conduit::blueprint::mesh::examples::braid("quads",100,100,0,data);
    
    //EXPECT_TRUE(conduit::blueprint::mesh::verify(data,verify_info));
   // verify_info.print();

    //
    // Create the actions.
    //
    
    string output_path = "";
    if(par_rank == 0)
    {
        output_path = prepare_output_dir();
    }
    else
    {
        output_path = output_dir();
    }
    output_path = conduit::utils::join_file_path(output_path,"test_save_adios");

    Node actions;
    Node &save = actions.append();
    save["action"]   = "save";
    save["output_path"] = output_path;
    save["selected_vars"] = "coordsets/coords/values";
    //actions.print();
    data.print();
    Node open_opts;
    open_opts["pipeline/type"] = "adios";
    open_opts["mpi_comm"] = MPI_Comm_c2f(comm);
    
    //
    // Run Strawman
    //
    Strawman sman;
    sman.Open(open_opts);
    sman.Publish(data);
    sman.Execute(actions);
    sman.Close();

}

int main(int argc, char* argv[])
{
    int result = 0;

    ::testing::InitGoogleTest(&argc, argv);
    MPI_Init(&argc, &argv);
    result = RUN_ALL_TESTS();
    MPI_Finalize();

    return result;
}

