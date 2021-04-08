/**
 * Copyright (C) Mellanox Technologies Ltd. 2021.  ALL RIGHTS RESERVED.
 *
 * See file LICENSE for terms.
 */

#include "config.h"
#include "components/tl/ucp/tl_ucp.h"
#include "components/tl/ucp/tl_ucp_coll.h"
#include "core/ucc_progress_queue.h"
#include "components/tl/ucp/tl_ucp_sendrecv.h"
#include "coll_patterns/recursive_knomial.h"
#include "coll_score/ucc_coll_score.h"
#include "utils/ucc_math.h"

#define UCC_TLCP_UCP_CUSTOM_SCORE 200
ucc_status_t ucc_tlcp_ucp_custom_progress(ucc_coll_task_t *coll_task)
{
    ucc_tl_ucp_task_t     *task       = ucc_derived_of(coll_task, ucc_tl_ucp_task_t);
//    ucc_tl_ucp_team_t     *team       = task->team;

    ucc_assert(UCC_TL_UCP_TASK_P2P_COMPLETE(task));
    task->super.super.status = UCC_OK;
    printf("Completing tl_ucp_custom coll task\n");
    return task->super.super.status;
}

ucc_status_t ucc_tlcp_ucp_custom_start(ucc_coll_task_t *coll_task)
{
    ucc_tl_ucp_task_t *task = ucc_derived_of(coll_task, ucc_tl_ucp_task_t);
    ucc_tl_ucp_team_t *team = task->team;
    printf("Starting tl_ucp_custom coll task\n");
    task->super.super.status = UCC_INPROGRESS;
    ucc_progress_enqueue(UCC_TL_UCP_TEAM_CORE_CTX(team)->pq, &task->super);
    return UCC_OK;
}

ucc_status_t ucc_tlcp_ucp_custom_coll_init(ucc_base_coll_args_t *coll_args,
                                             ucc_base_team_t *team,
                                             ucc_coll_task_t **task_h)
{
    ucc_tl_ucp_team_t    *tl_team = ucc_derived_of(team, ucc_tl_ucp_team_t);
    ucc_tl_ucp_task_t    *task    = ucc_tl_ucp_get_task(tl_team);
//    ucc_status_t          status;

    ucc_coll_task_init(&task->super);
    memcpy(&task->args, &coll_args->args, sizeof(ucc_coll_args_t));
    task->team           = tl_team;
    task->tag            = tl_team->seq_num;
    tl_team->seq_num     = (tl_team->seq_num + 1) % UCC_TL_UCP_MAX_COLL_TAG;
    task->super.finalize = ucc_tl_ucp_coll_finalize;
    task->super.post     = ucc_tlcp_ucp_custom_start;
    task->super.progress = ucc_tlcp_ucp_custom_progress;
    *task_h              = &task->super;
    return UCC_OK;
}

ucc_status_t ucc_tlcp_ucp_custom_get_scores(ucc_base_team_t   *tl_team,
                                              ucc_coll_score_t **score_p)
{
//    ucc_tl_ucp_team_t *team = ucc_derived_of(tl_team, ucc_tl_ucp_team_t);
//    ucc_tl_ucp_lib_t  *lib  = UCC_TL_UCP_TEAM_LIB(team);
    ucc_coll_score_t  *score;
    ucc_status_t       status;
    /* There can be a different logic for different coll_type/mem_type.
       Right now just init everything the same way. */
    status = ucc_coll_score_alloc(&score);
    if (UCC_OK != status) {
        printf("failed to alloc score\n");
        return status;
    }
    status = ucc_coll_score_add_range(score, UCC_COLL_TYPE_ALLTOALL, UCC_MEMORY_TYPE_HOST,
                                      65536, 131072, UCC_TLCP_UCP_CUSTOM_SCORE,
                                      ucc_tlcp_ucp_custom_coll_init, tl_team);
    if (UCC_OK != status) {
        printf("failed to add range\n");
        return status;
    }

    /* if (strlen(lib->super.super.score_str) > 0) { */
    /*     status = ucc_coll_score_update_from_str(lib->super.super.score_str, */
    /*                                             score, team->size); */
    /*     if (status == UCC_ERR_INVALID_PARAM) { */
    /*         /\* User provided incorrect input - try to proceed *\/ */
    /*         status = UCC_OK; */
    /*     } */
    /* } */
    *score_p = score;
    return status;
}

ucc_tl_coll_plugin_iface_t ucc_tlcp_ucp_custom = {
    .super.name = "tl_ucp_custom",
    .super.score = UCC_TLCP_UCP_CUSTOM_SCORE,
    .get_scores = ucc_tlcp_ucp_custom_get_scores
};
