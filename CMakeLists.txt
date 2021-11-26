if(NOT DEFINED ESP_PLATFORM)
    cmake_minimum_required(VERSION 3.19)
    project(dhyara)
    add_subdirectory(docs)
else()
    message(STATUS ENABLE_HTTP_MANAGEMENT_SERVER: ${CONFIG_ENABLE_HTTP_MANAGEMENT_SERVER})
    message(STATUS ENABLE_SERVICES_OVER_HTTP:     ${CONFIG_ENABLE_SERVICES_OVER_HTTP})

    set(DHYARA_ESSENTIAL_SOURCES
        "sources/address.cpp"
        "sources/peer.cpp"
        "sources/neighbour.cpp"
        "sources/neighbourhood.cpp"
        "sources/universe.cpp"
        "sources/link.cpp"
        "sources/synchronizer.cpp"
        "sources/network.cpp"
        "sources/routing/next_hop.cpp"
        "sources/routing/route.cpp"
        "sources/routing/mertic.cpp"
        "sources/routing/table.cpp"
        "sources/packets/beacon.cpp"
        "sources/packets/acknowledgement.cpp"
        "sources/packets/advertisement.cpp"
        "sources/packets/echo_request.cpp"
        "sources/packets/echo_reply.cpp"
        "sources/packets/echo_lost.cpp"
        "sources/packets/chunk.cpp"
        "sources/packets/data.cpp"
        "sources/packets/delivered.cpp"
        "sources/packets/io.cpp"
        "sources/actions/beacon.cpp"
        "sources/actions/acknowledgement.cpp"
        "sources/actions/advertisement.cpp"
        "sources/actions/echo_request.cpp"
        "sources/actions/echo_reply.cpp"
        "sources/actions/echo_lost.cpp"
        "sources/actions/chunk.cpp"
        "sources/actions/delivered.cpp"
        "sources/utils/printer.cpp"
        "sources/utils/udp_relay.cpp"
        "sources/tools/ping.cpp"
        "sources/tools/traceroute.cpp"
        "sources/dhyara.cpp"
        "sources/dhyara.c"
    )
    set(DHYARA_WWW_SOURCES
        "sources/utils/http.cpp"
    )
    set(DHYARA_WWW_SERVICES_SOURCES
        "sources/services/stream.cpp"
        "sources/services/identify.cpp"
    )

    if(CONFIG_ENABLE_SERVICE_ROUTES)
        set(DHYARA_WWW_SERVICES_SOURCES ${DHYARA_WWW_SERVICES_SOURCES} 
            "sources/services/routes.cpp"
        )
    endif()

    if(CONFIG_ENABLE_SERVICE_UNIVERSE)
        set(DHYARA_WWW_SERVICES_SOURCES ${DHYARA_WWW_SERVICES_SOURCES} 
            "sources/services/universe.cpp"
        )
    endif()

    if(CONFIG_ENABLE_SERVICE_PING)
        set(DHYARA_WWW_SERVICES_SOURCES ${DHYARA_WWW_SERVICES_SOURCES} 
            "sources/services/ping.cpp" 
            "sources/services/ping_impl.cpp"
        )
    endif()

    if(CONFIG_ENABLE_SERVICE_PHY_RATE)
        set(DHYARA_WWW_SERVICES_SOURCES ${DHYARA_WWW_SERVICES_SOURCES} 
            "sources/services/phy_rate.cpp"
        )
    endif()

    set(DHYARA_SOURCES ${DHYARA_ESSENTIAL_SOURCES})
    set(DHYARA_ASSETS )

    if(CONFIG_ENABLE_HTTP_MANAGEMENT_SERVER)
        set(DHYARA_SOURCES ${DHYARA_SOURCES} ${DHYARA_WWW_SOURCES})
        set(DHYARA_ASSETS ${DHYARA_ASSETS}
            "assets/min/head.html"
            "assets/min/dhyara.css"
            "assets/icons.gif"
        )
        if(CONFIG_ENABLE_SERVICES_OVER_HTTP)
            set(DHYARA_SOURCES ${DHYARA_SOURCES} ${DHYARA_WWW_SERVICES_SOURCES})
        endif()
    endif()

    idf_component_register(
        SRCS ${DHYARA_SOURCES}
        INCLUDE_DIRS "includes"
        REQUIRES cxx esp_wifi esp_http_server 
        EMBED_TXTFILES ${DHYARA_ASSETS}
    )

    # set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "time")
    # set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "time")
endif()
