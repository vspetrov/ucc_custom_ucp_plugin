#
# Copyright (C) Mellanox Technologies Ltd. 2020.  ALL RIGHTS RESERVED.
#


sources = custom.c

module_LTLIBRARIES = libucc_tlcp_ucp_custom.la
libucc_tlcp_ucp_custom_la_SOURCES  = $(sources)
libucc_tlcp_ucp_custom_la_CPPFLAGS = $(AM_CPPFLAGS) $(UCX_CPPFLAGS)
libucc_tlcp_ucp_custom_la_LDFLAGS  = -version-info $(SOVERSION) --as-needed $(UCX_LDFLAGS)
libucc_tlcp_ucp_custom_la_LIBADD   = $(UCX_LIBADD) $(UCC_TOP_BUILDDIR)/src/libucc.la \
                                      $(UCC_TOP_BUILDDIR)/src/components/tl/ucp/libucc_tl_ucp.la

include $(top_srcdir)/config/module.am
