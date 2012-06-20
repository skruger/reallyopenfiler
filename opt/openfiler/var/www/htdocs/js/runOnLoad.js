/*
 *
 * place any onload functions here .
 * always check the URL first to make
 * sure the right functions are called
 * on the right page
 *
 *
 */

function runOnLoad() {


    var url = window.location.pathname;

    // function list
   
    if (url == "/admin/quota.html") {

        onloadQuota();

    }

    else if (url == "/admin/quota_user.html") {

        onloadQuotaUser();

    }

    else if (url == "/admin/quota_guest.html") {

        onloadQuotaGuest();

    }

    else if (url == "/admin/services_ups.html") {

       onloadServicesUps();

    }

    else if (url == "/admin/system_ups.html") {

        onloadGeneralUps();

    }

    else if (url == "/admin/system_view_update.html") {

        onloadInitUpdatePage();

    }

    else if (url == "/admin/volumes_iscsi_targets.html") {

        onloadVolumesIscsiTargets();
    }

}
