
#ifndef HELPERS_H
#define HELPERS_H

#include <cmath>
#include <string>

#include <ROOT/RVec.hxx>

/**********************************************************/

using namespace ROOT::VecOps;

/* 
 * Helper namespace for functions.
 */


namespace Helper {

    /*
     * Package pt/eta/phi/mass into a vector.
     */
    ROOT::Math::PtEtaPhiMVector add_p4(float pt, float eta, float phi, float mass) {
        return ROOT::Math::PtEtaPhiMVector(pt, eta, phi, mass);
    }

    /*
     * Returns the difference in the azimuth coordinates of v1 and v2, taking
     * the boundary conditions at 2 * pi into account.
     */
    template <typename T>
    float DeltaPhi(T v1, T v2, const T c = M_PI) {
        auto r = std::fmod(v2 - v1, 2.0 * c);
        if (r < -c) {
        r += 2.0 * c;
        }
        else if (r > c) {
        r -= 2.0 * c;
        }
        return r;
    }

    /* 
     * Template function for computing Delta R.
     */
    template <typename T>
    float DeltaR(T eta1, T eta2, T phi1, T phi2) {
        auto deltaR = sqrt(pow(eta1 - eta2, 2) + pow(Helper::DeltaPhi(phi1, phi2), 2));
        return deltaR;
    }

    /**********************************************************/

    /*
     * Given two vectors of objects (could be muons+taus, could be jets+jets, etc.) return the indices of the leading pair.
     * For this example, define the "leading pair" as the pair where the objects are separated by deltaR > 0.4 and prioritize the pair that has the highest sum in pT.
    */
     std::vector<int> build_pair(RVec<int>& goodObjects_1, RVec<int>& goodObjects_2,
                                  RVec<float>& pt_1, RVec<float>& eta_1, RVec<float>& phi_1,
                                  RVec<float>& pt_2, RVec<float>& eta_2, RVec<float>& phi_2)  {

        // Get indices of all possible combinations
        auto comb = Combinations(pt_1, pt_2);
        const auto numComb = comb[0].size();

        // Tag which pairs are valid based on delta r
        std::vector<int> validPair(numComb, 0);
        for (size_t i = 0; i < numComb; i++) {
            const auto i1 = comb[0][i];
            const auto i2 = comb[1][i];
            if (goodObjects_1[i1] == 1 && goodObjects_2[i2] == 1) {
                const auto deltar = Helper::DeltaR(eta_1[i1], eta_2[i2], phi_1[i1], phi_2[i2]);
                if (deltar > 0.4) {
                    validPair[i] = 1;
                }
            }
        }

        // Prefer the pair with the highest pT sum
        int idx_1 = -1;
        int idx_2 = -1;
        float bestSum = -99;

        for (size_t i = 0; i < numComb; i++) {
            if (validPair[i] == 1) {
                const int tmp1 = comb[0][i];
                const int tmp2 = comb[1][i];
                if ((pt_1[tmp1] + pt_2[tmp2]) > bestSum) {
                    bestSum = (pt_1[tmp1] + pt_2[tmp2]);
                    idx_1 = tmp1;
                    idx_2 = tmp2;
                }
            }
        }


        return std::vector<int>({idx_1, idx_2});
    }

}
/**********************************************************/

#endif