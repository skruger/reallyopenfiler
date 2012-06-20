<?php

class DirectoryFilterDots extends FilterIterator {
    public function __construct($path) {
        parent::__construct(new RecursiveDirectoryIterator($path));
    }

    public function accept() {
        return !$this->getInnerIterator()->isDot();
    }
}

$webRootPath = "/opt/openfiler/var/www/htdocs"; 
$filterPath = new DirectoryFilterDots($webRootPath);
$filterDirPaths = array(); 

function listDirs($path, $webRootPath) {
   foreach ($path as $file) {
      if($file->isDir()) {
         $lpath = new DirectoryFilterDots($file);
         listDirs($lpath, $webRootPath);
      }
      else {
         global $filterDirPaths;
         $arr = explode($webRootPath, $file->__toString());
         $filterDirPaths[] = $arr[1];
      }
   }

   return $filterDirPaths;
}

$webRootFilter = listDirs($filterPath, $webRootPath);
?>
