#include <gsl/gsl_matrix.h>

#include "lfds_function.h"
#include "lfds_opt_problem.h"
#include "lfds_helper_functions.h"
#include "lfds_errors.h"


int
lfds_opt_problem_check_f(lfds_opt_problem_t *opt_problem)
{

    if (opt_problem->f == NULL || opt_problem->df == NULL || opt_problem->f_params == NULL) {
        opt_problem->status = CFPD_INVALID_F;
        GSL_ERROR(lfds_strerror(CFPD_INVALID_F), CFPD_INVALID_F);
    }

    return opt_problem->status;
}


int
lfds_opt_problem_check_bands(lfds_opt_problem_t *opt_problem)
{
    const gsl_matrix *P_min = opt_problem->P_min;
    const gsl_matrix *P_max = opt_problem->P_max;

    // Check that bands are defined
    if (P_min == NULL || P_max == NULL) {
        opt_problem->status = CFPD_INVALID_BANDS;
        GSL_ERROR(lfds_strerror(CFPD_INVALID_BANDS), CFPD_INVALID_BANDS);
    }

    const size_t N = opt_problem->N;
    const size_t K = opt_problem->K;
    const size_t N_min = opt_problem->P_min->size1;
    const size_t N_max = opt_problem->P_max->size1;
    const size_t K_min = opt_problem->P_min->size2;
    const size_t K_max = opt_problem->P_max->size2;
    const double mu = opt_problem->mu;

    // Check dimensions of P_min and P_max
    if (N_min != N || N_max != N || K_min != K || K_max != K) {
        opt_problem->status = CFPD_INVALID_BANDS;
        GSL_ERROR(lfds_strerror(CFPD_INVALID_BANDS), CFPD_INVALID_BANDS);
    }

    // Check validity of bands
    if (!lfds_matrix_geq(P_max, P_min)) {
        opt_problem->status = CFPD_INVALID_BANDS;
        GSL_ERROR(lfds_strerror(CFPD_INVALID_BANDS), CFPD_INVALID_BANDS);
    }

    for (size_t n = 0; n < N; n++) {
        gsl_vector_const_view view_min = gsl_matrix_const_row(P_min, n);
        gsl_vector_const_view view_max = gsl_matrix_const_row(P_max, n);

        // check pmin
        if (lfds_vector_sum(&view_min.vector) * mu > 1 || gsl_vector_min(&view_min.vector) < 0.0) {
            opt_problem->status = CFPD_INVALID_BANDS;
            GSL_ERROR(lfds_strerror(CFPD_INVALID_BANDS), CFPD_INVALID_BANDS);
        }

        // check pmax
        if (lfds_vector_sum(&view_max.vector) *mu < 1) {
            opt_problem->status = CFPD_INVALID_BANDS;
            GSL_ERROR(lfds_strerror(CFPD_INVALID_BANDS), CFPD_INVALID_BANDS);
        }
    }

    // Pmax must be finite
    if (!lfds_matrix_finite(P_max)) {
        opt_problem->status = CFPD_INVALID_BANDS;
        GSL_ERROR(lfds_strerror(CFPD_INVALID_BANDS), CFPD_INVALID_BANDS);
    }

    return opt_problem->status;
}


int
lfds_opt_problem_check_tolerances(lfds_opt_problem_t *opt_problem)
{
    if (opt_problem->eps_p <= 0.0 || opt_problem->eps_c <= 0.0 ||
            opt_problem->eps_objective <= 0.0 ||
            opt_problem->eps_densities <= 0.0) {
        opt_problem->status = CFPD_INVALID_TOLERANCES;
        GSL_ERROR(lfds_strerror(CFPD_INVALID_TOLERANCES), CFPD_INVALID_TOLERANCES);
    }

    return opt_problem->status;
}


int
lfds_opt_problem_check_P(lfds_opt_problem_t *opt_problem)
{
    const gsl_matrix *P_min = opt_problem->P_min;
    const gsl_matrix *P_max = opt_problem->P_max;
    const gsl_matrix *Q = opt_problem->Q;
    const size_t N = opt_problem->N;
    const size_t K = opt_problem->K;
    const size_t N_Q = opt_problem->Q->size1;
    const size_t K_Q = opt_problem->Q->size2;

    // Check that bands are specified
    if (P_min == NULL || P_max == NULL) {
        opt_problem->status = CFPD_INVALID_BANDS;
        GSL_ERROR(lfds_strerror(CFPD_INVALID_BANDS), CFPD_INVALID_BANDS);
    }

    // Check dimensions of P
    if (N_Q != N || K_Q != K) {
        opt_problem->status = CFPD_INVALID_P;
        GSL_ERROR(lfds_strerror(CFPD_INVALID_P), CFPD_INVALID_P);
    }

    // Check validity of bounds
    if (!lfds_matrix_geq(Q, P_min)) {
        opt_problem->status = CFPD_INVALID_P;
        GSL_ERROR(lfds_strerror(CFPD_INVALID_P), CFPD_INVALID_P);
    }
    if (!lfds_matrix_geq(P_max, Q)) {
        opt_problem->status = CFPD_INVALID_P;
        GSL_ERROR(lfds_strerror(CFPD_INVALID_P), CFPD_INVALID_P);
    }

    return opt_problem->status;
}
