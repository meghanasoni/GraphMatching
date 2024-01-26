#include <Matching.h>
#include <Vertex.h>
#include <GraphReader.h>
#include <CriticalRSM.h>
#include <Utils.h>
#include "TestDefs.h"

TEST_CASE("FavouriteNeighbour strict preference list", "[matching_critical_rsm]") {
    std::shared_ptr<Vertex> a1 = std::make_shared<Vertex>("a1");
    std::shared_ptr<Vertex> b1 = std::make_shared<Vertex>("b1");
    std::shared_ptr<Vertex> b2 = std::make_shared<Vertex>("b2");
    std::shared_ptr<Vertex> b3 = std::make_shared<Vertex>("b3");

    PreferenceList a1_pref_list;
    a1_pref_list.emplace_back(b1);
    a1_pref_list.emplace_back(b2);
    a1_pref_list.emplace_back(b3);

    VertexBookkeeping a1_data = VertexBookkeeping(0, 3, 0, 0);

    auto M = Matching();
    // required only for critical RSM constructor, doesn't affect output of the function
    auto G = read_graph(get_filepath(get_resources_dir(), "/weak_stable.txt"));
    CriticalRSM rsm(G);

    SECTION("strict preference list") {
        auto v = rsm.favourite_neighbour(a1, a1_pref_list, a1_data, M);

        REQUIRE(v == b1);
    }
}

TEST_CASE("FavouriteNeighbour tied preference list", "[matching_critical_rsm]") {
    std::shared_ptr<Vertex> a1 = std::make_shared<Vertex>("a1");
    std::shared_ptr<Vertex> b1 = std::make_shared<Vertex>("b1");
    std::shared_ptr<Vertex> b2 = std::make_shared<Vertex>("b2");
    std::shared_ptr<Vertex> b3 = std::make_shared<Vertex>("b3");
    std::shared_ptr<Vertex> b4 = std::make_shared<Vertex>("b4");
    // dummy partner for all matched vertices
    std::shared_ptr<Vertex> dummy = std::make_shared<Vertex>("d");

    // required only for critical RSM constructor, doesn't affect output of the function
    auto G = read_graph(get_filepath(get_resources_dir(), "/weak_stable.txt"));
    CriticalRSM rsm(G);

    SECTION("Case 1") {
        // b1: rank 1, proposed, matched
        // b2: rank 2, proposed, matched
        // b3: rank 2, unproposed, matched
        // b4: rank 2, unproposed, unmatched -> favourite neighbour
        PreferenceList a1_pref_list;
        a1_pref_list.emplace_back(b1);
        a1_pref_list.set_tie(1, b2);
        a1_pref_list.set_tie(1, b3);
        a1_pref_list.set_tie(1, b4);
        VertexBookkeeping a1_data = VertexBookkeeping(1, 2, 1, 0);

        auto M = Matching();
        M.add_partner(b1, Partner(dummy, 1, 0), 0);
        M.add_partner(b2, Partner(dummy, 1, 0), 0);
        M.add_partner(b3, Partner(dummy, 1, 0), 0);

        auto v = rsm.favourite_neighbour(a1, a1_pref_list, a1_data, M);
        REQUIRE(v == b4);
    }
    SECTION("Case 2") {
        // b1: rank 1, proposed, matched
        // b2: rank 2, proposed, matched
        // b3: rank 2, unproposed, matched -> favourite neighbour
        // b4: rank 2, unproposed, umatched
        PreferenceList a1_pref_list;
        a1_pref_list.emplace_back(b1);
        a1_pref_list.set_tie(1, b2);
        a1_pref_list.set_tie(1, b3);
        a1_pref_list.set_tie(1, b4);
        VertexBookkeeping a1_data = VertexBookkeeping(1, 2, 1, 0);

        auto M = Matching();
        M.add_partner(b1, Partner(dummy, 1, 0), 0);
        M.add_partner(b2, Partner(dummy, 1, 0), 0);
        M.add_partner(b3, Partner(dummy, 1, 0), 0);
        M.add_partner(b4, Partner(dummy, 1, 0), 0);

        auto v = rsm.favourite_neighbour(a1, a1_pref_list, a1_data, M);
        REQUIRE(v == b3);
    }
    SECTION("Case 3") {
        // b1: rank 1, proposed, matched
        // b2: rank 2, proposed, matched
        // b3: rank 2, proposed, matched, marked -> favourite neighbour
        // b4: rank 3, unproposed, matched
        PreferenceList a1_pref_list;
        a1_pref_list.emplace_back(b1);
        a1_pref_list.set_tie(1, b2);
        a1_pref_list.set_tie(1, b3);
        a1_pref_list.emplace_back(b4);

        VertexBookkeeping a1_data = VertexBookkeeping(2, 3, 0, 0);
        a1_data.marked[b3] = true;

        auto M = Matching();
        M.add_partner(b1, Partner(dummy, 1, 0), 0);
        M.add_partner(b2, Partner(dummy, 1, 0), 0);
        M.add_partner(b3, Partner(dummy, 1, 0), 0);
        M.add_partner(b4, Partner(dummy, 1, 0), 0);

        auto v = rsm.favourite_neighbour(a1, a1_pref_list, a1_data, M);
        REQUIRE(v == b3);
    }
}

TEST_CASE("Prefers method of PreferenceList", "[pref_list]") {
    std::shared_ptr<Vertex> b1 = std::make_shared<Vertex>("b1");
    std::shared_ptr<Vertex> b2 = std::make_shared<Vertex>("b2");
    std::shared_ptr<Vertex> b3 = std::make_shared<Vertex>("b3");

    PreferenceList a1_pref_list;
    a1_pref_list.emplace_back(b1);
    a1_pref_list.set_tie(1, b2);
    a1_pref_list.set_tie(1, b3);

    auto preference1 = a1_pref_list.prefers(b1, b3);
    REQUIRE(preference1 == cBetter);

    auto preference3 = a1_pref_list.prefers(b2, b3);
    REQUIRE(preference3 == cEqual);

    auto preference2 = a1_pref_list.prefers(b3, b1);
    REQUIRE(preference2 == cWorse);
}