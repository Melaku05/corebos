<?php
/*+**********************************************************************************
 * The contents of this file are subject to the vtiger CRM Public License Version 1.0
 * ("License"); You may not use this file except in compliance with the License
 * The Original Code is:  vtiger CRM Open Source
 * The Initial Developer of the Original Code is vtiger.
 * Portions created by vtiger are Copyright (C) vtiger.
 * All Rights Reserved.
 ************************************************************************************/

class VTWorkflowApplication {
	public function __construct($action, $returnUrl = false) {
		global $_REQUEST;
		$this->name = 'com_vtiger_workflow';
		$this->label = 'Workflow';
		$this->action = $action;
		if ($returnUrl===false) {
			$returnUrl='index.php?'.$_SERVER['QUERY_STRING'];
		}
		$this->returnUrl = urldecode($returnUrl);
	}

	public function currentUrl() {
		return $_SERVER['REQUEST_URI'];
	}

	public function returnUrl() {
		return $this->returnUrl;
	}

	public function listViewUrl() {
		return "index.php?module={$this->name}&action=workflowlist";
	}

	public function editWorkflowUrl($id = null) {
		if ($id!=null) {
			$idPart="&workflow_id=$id";
		} else {
			$idPart = '';
		}
		return "index.php?module={$this->name}&action=editworkflow$idPart&return_url=".urlencode($this->returnUrl());
	}

	public function deleteWorkflowUrl($id, $withReturn = true) {
		$idPart = Vtiger_Request::addCRSF2URL("&workflow_id=$id");
		return "index.php?module={$this->name}&action=deleteworkflow$idPart".($withReturn ? '&return_url='.urlencode($this->returnUrl()) : '');
	}

	public function editTaskUrl($id = null) {
		if ($id!=null) {
			$idPart="&task_id=$id";
		}
		return "index.php?module={$this->name}&action=edittask$idPart&return_url=".urlencode($this->returnUrl());
	}

	public function deleteTaskUrl($id) {
		$idPart = "&task_id=$id&".$GLOBALS['csrf']['input-name'].'='.csrf_get_tokens();
		return "index.php?module={$this->name}&action=deletetask$idPart&return_url=".urlencode($this->returnUrl());
	}

	public function onoffTaskUrl($id, $active = 0) {
		$idPart = "&task_id=$id&".$GLOBALS['csrf']['input-name'].'='.csrf_get_tokens();
		return "index.php?module={$this->name}&action=onofftask$idPart&isactive=$active&return_url=".urlencode($this->returnUrl());
	}

	public function setReturnUrl($returnUrl) {
		$this->returnUrl = $returnUrl;
	}

	public function errorPageUrl($message) {
		return "index.php?module={$this->name}&action=errormessage&message=".urlencode($message);
	}

	public function activatedeactivateTaskUrl($id, $active) {
		$idPart="&workflow_id=$id&".$GLOBALS['csrf']['input-name'].'='.csrf_get_tokens();
		return "index.php?module={$this->name}&action=activatedeactivateTask$idPart&active=$active&return_url=".urlencode($this->returnUrl());
	}
}
?>