project(GoHistogram NONE)
set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.go)
add_custom_command(OUTPUT go-histogram
	COMMAND go build -o go-histogram ${SOURCES}
	DEPENDS ${SOURCES})
add_custom_target(go-histogram-exe ALL DEPENDS go-histogram)
