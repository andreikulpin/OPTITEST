/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   exprfunc.hpp
 * Author: alusov
 *
 * Created on February 27, 2017, 9:58 AM
 */

#ifndef EXPRFUNC_HPP
#define EXPRFUNC_HPP

#include <vector>
#include <memory>
#include "expression/expr.hpp"
#include "expression/algorithm.hpp"
#include "testfuncs/manydim/benchmarks.hpp"

#include <mpproblem.hpp>
#include <box/box.hpp>

using namespace snowgoose::expression;

namespace OPTITEST {

    class BenchmarkFunctor : public COMPI::Functor <double> {
    public:

        BenchmarkFunctor(const PtrBench<double> benchmark) : bm(benchmark) {
        }

        double func(const double* x) const {
            std::vector<double> v(x, x+bm->getDim());
            return bm->calcFunc(v);            
        }

	void grad(const double* x, double* g) {
            std::vector<double> v(x, x+bm->getDim());
	    auto gr = bm->calcGrad(v).grad();
	    memcpy(g, gr.getGrad(), bm->getDim() * sizeof(double));		
        }
                
    private:
	PtrBench<double> bm;
    };
    
    /**
     * Factory to produce instances of any expression optimization problem
     */
    class BenchmarkProblemFactory {
    public:

        BenchmarkProblemFactory(const PtrBench<double> benchmark) : bm(benchmark) {
        }

        COMPI::MPProblem<double>* getProblem() const {
	    int dim = bm->getDim();
            COMPI::MPProblem<double>* prob = new COMPI::MPProblem<double>();
            prob->mVarTypes.assign(dim, COMPI::MPProblem<double>::VariableTypes::GENERIC);
            std::shared_ptr<COMPI::Functor <double>> funcPtr = std::make_shared<BenchmarkFunctor>(BenchmarkFunctor(bm));
            std::shared_ptr<COMPI::Functor <double>> funcCntPtr = std::make_shared<COMPI::FuncCnt<double>>(funcPtr);
            prob->mObjectives.push_back(funcCntPtr);
            prob->mBox = new snowgoose::Box<double>(dim);
            for (int i = 0; i < dim; i++) {
                prob->mBox->mA[i] = bm->getBounds()[i].first;
                prob->mBox->mB[i] = bm->getBounds()[i].second;
            }
            return prob;
        }

    private:
	PtrBench<double> bm;
    };
}

#endif /* EXPRFUNC_HPP */

