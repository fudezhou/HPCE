    #include <cstdlib>                      // System includes
    #include <iostream>                      
    #include <Eigen/SparseCore>
    #include <Eigen/IterativeLinearSolvers>
    #include <unsupported/Eigen/SparseExtra>

    using std::endl;
    using std::cout;

    #include "cg.hpp"                          

    int main(int argc, char** argv)
    {
    using namespace LinearAlgebra;
    // Some useful alias
    using SpMat=Eigen::SparseMatrix<double>;
    using SpVec=Eigen::VectorXd;

    //   int n = 1000;
    //   SpMat M(n,n);                       // define matrix
    //   M.reserve(2998);
    //   for (int i=0; i<n; i++) {
    //       M.coeffRef(i, i) = 2.0*(i+1);
    //       if(i>0) M.coeffRef(i, i-1) = -i;
    //       if(i<n-1) M.coeffRef(i, i+1) = -(i+1);
    //   }

    SpMat M;
    Eigen::loadMarket(M, "bcsstm12.mtx");

    M = M + SpMat(M.transpose());

    int n = M.rows();

        


    double tol = 1.e-10;                 // Convergence tolerance
    int result, maxit = 1000;           // Maximum iterations

    std::cout<<"Matrix size:"<<M.rows()<<"X"<<M.cols()<<std::endl;
    std::cout<<"Non zero entries:"<<M.nonZeros()<<std::endl;

    SpMat B = SpMat(M.transpose()) - M;  // Check symmetry
    std::cout<<"Norm of M-M.t: "<<B.norm()<<std::endl;

    // Create Rhs b
    SpVec e=SpVec::Ones(M.rows());
    SpVec b=M* e;
    SpVec x(M.rows());
    Eigen::DiagonalPreconditioner<double> D(M); // Create diagonal preconditioner
    Eigen::IncompleteLUT<double> ILUT(M);

    // First with eigen CG
    Eigen::ConjugateGradient<SpMat, IncompleteLUT> cg;
    cg.setMaxIterations(maxit);
    cg.setTolerance(tol);
    cg.compute(M);
    x = cg.solve(b);
    std::cout <<" Eigen native CG"<<std::endl;
    std::cout << "#iterations:     " << cg.iterations() << std::endl;
    std::cout << "estimated error: " << cg.error()      << std::endl;
    std::cout << "effective error: "<<(x-e).norm()<<std::endl;

    // Now with hand-made CG
    x=0*x;
    result = CG(M, x, b, ILUT, maxit, tol);        // Solve system

    std::cout <<" hand-made CG "<<std::endl;
    cout << "CG flag = " << result << endl;
    cout << "iterations performed: " << maxit << endl;
    cout << "tolerance achieved  : " << tol << endl;
    std::cout << "Error norm: "<<(x-e).norm()<<std::endl;

    return result;
    }